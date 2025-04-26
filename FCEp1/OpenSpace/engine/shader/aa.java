
    float shadow = 0.2f;

    float sX = feLightActual.x - fragRealPos.x;
    float sY = feLightActual.y - fragRealPos.y;

    float xI = sX / 128.0f;
    float yI = sY / 128.0f;

    shadow = 1.0f;

    float aX = fragRealPos.x;
    float aY = fragRealPos.y;

    for(int i=0;i<128;i++){

        aX += xI;
        aY += yI;

        vec2 sUV = vec2(aX / feMapSize.x,1.0-aY/feMapSize.y);

        float sv = texture(feShadowMap,sUV).r;
        if(sv<0.1)
        {
            shadow=0.0f;

        }

    }