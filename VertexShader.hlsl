// an ultra simple hlsl vertex shader
// TODO: Part 1c
// TODO: Part 2b
#pragma pack_matrix(row_major)

cbuffer ShaderVars : register(b0)
{
    matrix worldMatrix;
    float padding[44];
};
	// TODO: Part 3b
	// TODO: Part 3f
// TODO: Part 3g
float4 main(float4 inputVertex : POSITION) : SV_POSITION
{
	// TODO: Part 2i
    return mul(inputVertex, worldMatrix);
	// TODO: Part 3b
	// TODO: Part 3g
}