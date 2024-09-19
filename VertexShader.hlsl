// an ultra simple hlsl vertex shader
// TODO: Part 1c
// TODO: Part 2b
#pragma pack_matrix(row_major)

cbuffer ShaderVars : register(b0)
{
    matrix worldMatrix[6];
    matrix viewMatrix;
    //float padding[28];
    matrix projectionMatrix;
		float padding[12];
    
};
	// TODO: Part 3b
	// TODO: Part 3f
// TODO: Part 3g
float4 main(float4 inputVertex : POSITION, uint matrix_index : SV_InstanceID) : SV_POSITION
{
	// TODO: Part 2i
    float4 worldPos  = mul(inputVertex, worldMatrix[matrix_index]);
	// TODO: Part 3b
	float4 viewPos = mul(worldPos, viewMatrix);
    float4 projectedPosition = mul(viewPos, projectionMatrix); // Apply projection

    return projectedPosition;
	// TODO: Part 3g
}