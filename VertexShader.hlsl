// an ultra simple hlsl vertex shader
// TODO: Part 1c
// TODO: Part 2b
//cbuffer SHADER_VARS : register(b0)
//{
//    matrix worldMatrix;
//    float padding[32];
//};
//	// TODO: Part 3b
//	// TODO: Part 3f
//// TODO: Part 3g
//float4 main(float4 inputVertex : POSITION) : SV_POSITION
//{
//	// TODO: Part 2i
//	// TODO: Part 3b
//	// TODO: Part 3g
//	return float4(inputVertex);
//}


cbuffer SHADER_VARS : register(b0)
{
    float4x4 worldMatrix;
    float padding[32];
};

struct VS_INPUT
{
    float4 position : POSITION;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
};

#pragma pack_matrix(row_major)

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = mul(input.position, worldMatrix);
    return output;
}
