struct VS_Input {
    uint id : SV_VertexID;
};

struct VS_Output {
    float4 pos : SV_POSITION;
    float4 Color : Color;
};

static float2 pos[] = {
    float2(-0.5, -0.5),
    float2( 0.0,  0.5),
    float2( 0.5, -0.5)
};

VS_Output main(VS_Input IN){
    VS_Output OUT = (VS_Output) 0;

    OUT.pos = float4(pos[IN.id], 0.0, 1.0);
    OUT.Color = clamp(OUT.pos, 0, 1);
    return OUT;
}