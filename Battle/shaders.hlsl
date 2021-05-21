//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

cbuffer cbPerObject : register(b0)
{
    float4x4 gWorldViewProj;
    float4 c[48];
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PSInput VSMain(float4 position : POSITION, float4 color : COLOR, uint4 rindex : RINDEX)
{
    PSInput result;

    result.position = mul(gWorldViewProj, position);
    result.position.z = 0;
    result.color = color + c[rindex[0]];

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return input.color;
}
