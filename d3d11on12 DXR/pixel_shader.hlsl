struct PS_Input {
    float4 pos : SV_POSITION;
    linear float4 Color : Color;
};

struct PS_Output {
    float4 Color: SV_TARGET;
};

PS_Output main(PS_Input IN) {
    PS_Output OUT = (PS_Output) 0;

    OUT.Color = IN.Color;
    return OUT;
}