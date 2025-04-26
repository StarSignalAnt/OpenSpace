#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <functional>
class Texture2D;
class IWindow;
class IMainMenu;

enum HitType {

	HT_True,HT_False

};

enum DockType {

	m_Fill, m_Left, m_Right, m_Down, m_Up, m_Free,
	m_Centre

};

class IControl
{
public:

	IControl() : m_Position(0, 0), m_Size(0, 0),m_Color(glm::vec4(1,1,1,1)) {}
	IControl(glm::vec2 position, glm::vec2 size) : m_Position(position), m_Size(size),m_Color(glm::vec4(1,1,1,1)) {}
	void Set(glm::vec2 position, glm::vec2 size = glm::vec2(-1, -1),bool transist = false)
	{
		transist = false;
		if (size.x < 0) {
			size = m_Size;
		}
		else {
		
		}
		if (transist) {
			m_Transist = true;
			m_TargetPosition = position;
			m_TargetSize = size;
			return;
		}
		
		m_Position = position;
		m_Size = size;

		AfterSet();
		
	}
	void SetRoot(IControl* root) { m_RootControl = root; }
	IControl* GetRoot() { return m_RootControl; }
	virtual void AddChild(IControl* child) { m_Children.push_back(child); child->SetRoot(this); ApplyDockChildren(); }
	void RemoveChild(IControl* child) { m_Children.erase(std::remove(m_Children.begin(), m_Children.end(), child), m_Children.end()); }
	void SetImage(Texture2D* image) { m_Image = image; }

	Texture2D* GetImage() { return m_Image; }
	void SetText(std::string text) { m_Text = text; }
	std::string GetText() { return m_Text; }
	glm::vec2 GetRenderPosition();
	std::vector<IControl*> GetChildren() { return m_Children; }

	void setScissor(int x, int y, int width, int height, int windowHeight);

	virtual void Update(float delta) = 0;
	virtual void Render() = 0;
	virtual void PreRender() {
		PreRenderChildren();
	}
	void UpdateChildren(float delta);
	void RenderChildren();
	void PreRenderChildren();
	virtual void OnMouseEnter() {};
	virtual void OnMouseLeave() {};
	virtual void OnMouseDown(int button) {};
	virtual void OnMouseUp(int button) {};
	virtual void OnMouseMove(glm::vec2 position,glm::vec2 delta) {};
	virtual void OnMouseWheel(float delta) {};
	virtual void OnKeyDown(int key) {};
	virtual void OnKeyUp(int key) {};
	virtual void OnMouseDoubleClick() {};
	virtual void OnActivate() {};
	virtual void OnDeactivate() {}
	virtual bool InBounds(glm::vec2 position)
	{
		if (m_HitType == HT_False) return false;
		glm::vec2 root = GetRenderPosition();
		

		if (GetRoot()) {
			if (GetRoot()->GetCullChildren()) {
				if (m_RootControl != nullptr) {
					if (position.x < m_RootControl->GetRenderPosition().x || position.y < m_RootControl->GetRenderPosition().y)
					{
						return false;

					}
					if (position.x > m_RootControl->GetRenderPosition().x + m_RootControl->GetSize().x || position.y > m_RootControl->GetRenderPosition().y + m_RootControl->GetSize().y) {
						return false;
					}
				}
			}

		}

		if (position.x > root.x && position.x < root.x + m_Size.x &&
			position.y > root.y && position.y < root.y + m_Size.y)
		{
			return true;
		}
		return false;
	}
	void Click(void* data=nullptr) {
		if (OnClick) {
			OnClick(data);  // Call the assigned function
		}
	}
	void DoubleClick() {
		if (OnDoubleClick) {
			OnDoubleClick();
		}	
	}
	void CallPreRender() {
		if (OnPreRender) {
			OnPreRender();
		}
	}

	void Enter(void * data) {
		if (OnEnter) {
			OnEnter(data);
		}
	}

	void Move(glm::vec2 delta) {

		if (OnMove) {
			OnMove(delta);
		}
	}
	
	void SetOnClick(std::function<void(void*)> callback) {
		OnClick = callback;
	}
	void SetOnDoubleClick(std::function<void()> callback)
	{
	
		OnDoubleClick = callback;
	}
	void SetOnPreRender(std::function<void()> callback) {

		OnPreRender = callback;

	}
	void SetOnMoved(std::function<void(glm::vec2)> move)
	{
		OnMove = move;

	}
	void SetOnEnter(std::function<void(void*)> enter) {

		OnEnter = enter;

	}

	


	int GetMaxHeight();
	int GetMaxWidth();

	glm::vec2 GetPosition() { return m_Position; }
	glm::vec2 GetSize() { return m_Size; }
	glm::vec2 GetOffset() { return m_Offset; }
	void SetOffset(glm::vec2 offset) { m_Offset = offset; };
	void SetCullChildren(bool cull) { m_CullChildren = cull; }
	virtual void AfterSet() {};
	bool IsWindow(bool title_only = true);
	IWindow* GetWindow();
	void SetDockType(DockType type) {
		m_DockType = type;
	}
	DockType GetDockType() {
		return m_DockType;
	}
	void ApplyDockChildren() {


		for (auto child : m_Children) {
			switch (child->GetDockType()) {
			case DockType::m_Centre:

				child->Set(glm::vec2(0, child->GetPosition().y), glm::vec2(m_Size.x, child->GetSize().y));

				break;
			case DockType::m_Fill:

				//child->Set(child->GetPosition(), m_Size-child->GetPosition());
				child->Set(glm::vec2(0, 0), m_Size);

				break;
			case DockType::m_Down:

				child->Set(glm::vec2(0, m_Size.y - child->GetSize().y), glm::vec2(m_Size.x, child->GetSize().y));

				break;
			case DockType::m_Up:

				child->Set(glm::vec2(0, 0), glm::vec2(m_Size.x, child->GetSize().y));

				break;
			}
			child->ApplyDockChildren();
		}
	}
	bool GetTransist() {
		return m_Transist;
	}
	void SetTransist(bool transist) {
		m_Transist = transist;
	}
	glm::vec2 GetTargetPosition() {
		return m_TargetPosition;
	}
	glm::vec2 GetTargetSize() {
		return m_TargetSize;
	}
	bool GetCullChildren() {
		return m_CullChildren;
	}
	void SetData(void* data) {
		m_Data = data;
	}

	void SetHitType(HitType type) {

		m_HitType = type;

	}

	HitType GetHitType() {

		return m_HitType;

	}

	bool IsChild(IControl* control) {

		for (auto child : m_Children) {

			if (child == control) return true;

			if (child->IsChild(control)) return true;

		}


		if (control == this) return true;

		return false;

	}


	void SetOnMousePress(std::function<void(int button)> func) {
		OnMousePress = func;
	}

	void SetOnMouseUnPress(std::function<void(int button)> func) {
		OnMouseUnPress = func;
	}

	void MousePressed(int button) {

		if (OnMousePress) {
			OnMousePress(button);
		}
	}

	void MouseUnPressed(int button) {

		if (OnMouseUnPress) {
			OnMouseUnPress(button);
		}

	}

	void SetOnMouseWheel(std::function<void(float move)> func) {

		OnMouseWheelMove = func;

	}

	void MouseWheelMoved(float v) {

		if (OnMouseWheelMove) {

			OnMouseWheelMove(v);

		}

	}

	bool GetIgnoreScissor() const { return m_IgnoreScissor; }
	void SetIgnoreScissor(bool ignore) { m_IgnoreScissor = ignore; }

	// In the protected section:
	bool m_IgnoreScissor = false; // Controls whether scissor testing affects this control

	glm::vec4 GetScissor();

	virtual void Setup() {};
	void ClearChildren() {

		m_Children.clear();

	}

protected:


	void* m_Data = nullptr;
	glm::vec2 m_Position;
	glm::vec2 m_Size;
	glm::vec2 m_Offset;
	std::vector<IControl*> m_Children;
	IControl* m_RootControl = nullptr;
	Texture2D* m_Image = nullptr;
	std::string m_Text;
	glm::vec4 m_Color;
	std::function<void(void* data)> OnClick = nullptr;
	std::function<void()> OnDoubleClick = nullptr;
	std::function<void()> OnPreRender = nullptr;
	std::function<void(glm::vec2 delta)> OnMove = nullptr;
	std::function<void(void *)> OnEnter = nullptr;
	std::function<void(glm::vec2 pos, glm::vec2 delta)> OnMouseMoved = nullptr;
	std::function<void(int button)> OnMousePress = nullptr;
	std::function<void(int button)> OnMouseUnPress = nullptr;
	std::function<void(float move)> OnMouseWheelMove = nullptr;

	bool m_CullChildren = false;
	DockType m_DockType = DockType::m_Free;
	bool m_Transist = false;
	glm::vec2 m_TargetPosition;
	glm::vec2 m_TargetSize;
	IMainMenu* m_ActiveMenu = nullptr;
	HitType m_HitType = HitType::HT_True;
};

