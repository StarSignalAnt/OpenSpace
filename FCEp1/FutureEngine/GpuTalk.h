#include <string>
#include <vector>
#include <iostream>
#include "llama.h"
#include <thread>
#include <math.h>
#include <random>

class GpuTalk {
private:
    // Model and context pointers
    llama_model* model;
    llama_context* ctx;
    const llama_vocab* vocab;
    llama_sampler* sampler;

    // Configuration parameters
    int max_tokens;
    int context_size;
    int rseed = 0;

public:

    float getRandomFloat() {
        // Get current time in microseconds
        unsigned seed = clock();

        // Create random number generator with time-based seed
        std::default_random_engine generator(seed);

        // Create uniform distribution between 0 and 1
        std::uniform_real_distribution<float> distribution(0.0f, 1.0f);

        // Generate and return random float
        return distribution(generator);
    }


    // Constructor - loads the model and initializes everything
    GpuTalk(const std::string& model_path, unsigned int seed = std::random_device{}(), int ctx_size = 2048 * 2, int max_response_tokens = 4096)
        : max_tokens(max_response_tokens), context_size(ctx_size), rseed(seed) {

        // Initialize llama backend
        llama_backend_init();
        std::cout << "Rnd:" << getRandomFloat() << std::endl;

   
        // Configure model parameters with GPU acceleration
        llama_model_params model_params = llama_model_default_params();
        model_params.n_gpu_layers = INT32_MAX;  // Offload as many layers as possible to GPU
        model_params.main_gpu = 0;              // Use the primary GPU
        model_params.use_mmap = true;           // Use memory mapping for faster loading

        // Load the model
        model = llama_model_load_from_file(model_path.c_str(), model_params);
        if (!model) {
            throw std::runtime_error("Failed to load model: " + model_path);
        }

        // Configure context parameters
        llama_context_params ctx_params = llama_context_default_params();
        ctx_params.n_ctx = context_size;       // Set context window size
        ctx_params.n_batch = 512;              // Batch size for processing multiple tokens
        ctx_params.n_ubatch = 512;             // Physical batch size

        // Use multiple threads for faster processing
        int n_threads = 4;// std::min(8, (int)std::thread::hardware_concurrency());
        ctx_params.n_threads = n_threads;
        ctx_params.n_threads_batch = n_threads;

        // Enable optimizations
        ctx_params.flash_attn = true;          // Use flash attention if available

        // Initialize the context
        ctx = llama_init_from_model(model, ctx_params);
        if (!ctx) {
            llama_model_free(model);
            throw std::runtime_error("Failed to create context");
        }

        // Get vocabulary for tokenization
        vocab = llama_model_get_vocab(model);

        // Initialize the sampler chain
        llama_sampler_chain_params sparams = llama_sampler_chain_default_params();
        sampler = llama_sampler_chain_init(sparams);

        // Add sampling strategies
        llama_sampler_chain_add(sampler, llama_sampler_init_top_k(40));
        llama_sampler_chain_add(sampler, llama_sampler_init_top_p(0.95, 1));
        llama_sampler_chain_add(sampler, llama_sampler_init_temp(0.6+getRandomFloat()*0.5));
        llama_sampler_chain_add(sampler, llama_sampler_init_dist(0));
    }

    // Destructor - cleans up resources
    ~GpuTalk() {
        if (sampler) llama_sampler_free(sampler);
        if (ctx) llama_free(ctx);
        if (model) llama_model_free(model);
        // No need to free vocab as it's owned by the model
    }

    // Ask a question and get a response as a string
    std::string Ask(const std::string& question) {
        // Format the prompt
        std::string prompt = question;

        // Tokenize the prompt
        std::vector<llama_token> tokens(context_size);
        int n_tokens = llama_tokenize(vocab, prompt.c_str(), prompt.length(),
            tokens.data(), tokens.size(), true, false);

        if (n_tokens < 0) {
            return "Error: Failed to tokenize input";
        }

        tokens.resize(n_tokens);

        // Process the input tokens
        llama_batch batch = llama_batch_get_one(tokens.data(), tokens.size());

        if (llama_decode(ctx, batch) != 0) {
            llama_batch_free(batch);
            return "Error: Failed to process input";
        }

       // llama_batch_free(batch);

        // Generate response tokens
        std::vector<llama_token> response_tokens;
        llama_token token = 0;

        for (int i = 0; i < max_tokens; i++) {
            // Sample a token
            token = llama_sampler_sample(sampler, ctx, -1);

            // Check for end of generation
            if (llama_vocab_is_eog(vocab, token)) {
                break;
            }

            response_tokens.push_back(token);

            // Accept the token
            llama_sampler_accept(sampler, token);

            // Prepare the next batch with this token
            batch = llama_batch_get_one(&token, 1);

            // Decode this token
            if (llama_decode(ctx, batch) != 0) {
                llama_batch_free(batch);
                return "Error: Failed to process generated token";
            }

           // llama_batch_free(batch);
        }

        // Convert response tokens to text
        std::string result;
        char buffer[256];

        for (const auto& token : response_tokens) {
            int n = llama_token_to_piece(vocab, token, buffer, sizeof(buffer) - 1, 0, true);
            if (n > 0) {
                buffer[n] = 0;
                result += buffer;
            }
        }

        return result;
    }
    std::string AskWithContext(const std::string& context, const std::string& question) {
        // Format the prompt with the context followed by the question
        std::string formatted_prompt = context + "\n\nQuestion: " + question;

        // Use the existing Ask method to handle all the llama processing
        return Ask(formatted_prompt);
    }

    // Method to ask a question with examples for few-shot learning
    std::string AskWithExamples(const std::vector<std::pair<std::string, std::string>>& examples,
        const std::string& question) {
        std::string formatted_prompt = "";

        // Format each example as Q&A pair
        for (const auto& example : examples) {
            formatted_prompt += "Question: " + example.first + "\nAnswer: " + example.second + "\n\n";
        }

        // Add the current question - no need to add "Question:" prefix since Ask will add it
        formatted_prompt += question;

        // Use the existing Ask method to handle all the llama processing
        return Ask(formatted_prompt);
    }
    // Alternative version that returns a vector of strings (one per token)
    std::vector<std::string> AskVerbose(const std::string& question) {
        // Format the prompt
        std::string prompt = "Question: " + question + "\nAnswer:";

        // Tokenize the prompt
        std::vector<llama_token> tokens(context_size);
        int n_tokens = llama_tokenize(vocab, prompt.c_str(), prompt.length(),
            tokens.data(), tokens.size(), true, false);

        if (n_tokens < 0) {
            return { "Error: Failed to tokenize input" };
        }

        tokens.resize(n_tokens);

        // Process the input tokens
        llama_batch batch = llama_batch_get_one(tokens.data(), tokens.size());

        if (llama_decode(ctx, batch) != 0) {
            llama_batch_free(batch);
            return { "Error: Failed to process input" };
        }

        llama_batch_free(batch);

        // Generate response tokens
        std::vector<std::string> result;
        llama_token token = 0;

        for (int i = 0; i < max_tokens; i++) {
            // Sample a token
            token = llama_sampler_sample(sampler, ctx, -1);

            // Check for end of generation
            if (llama_vocab_is_eog(vocab, token)) {
                break;
            }

            // Accept the token
            llama_sampler_accept(sampler, token);

            // Convert token to text
            char buffer[256];
            int n = llama_token_to_piece(vocab, token, buffer, sizeof(buffer) - 1, 0, true);
            if (n > 0) {
                buffer[n] = 0;
                result.push_back(std::string(buffer));
            }

            // Prepare the next batch with this token
            batch = llama_batch_get_one(&token, 1);

            // Decode this token
            if (llama_decode(ctx, batch) != 0) {
                llama_batch_free(batch);
                result.push_back("Error: Failed to process generated token");
                return result;
            }

            llama_batch_free(batch);
        }

        return result;
    }
};