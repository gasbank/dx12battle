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

#define GRID_COUNT_X (8*4)
#define GRID_COUNT_Y (6*4)

cbuffer cbPerObject : register(b0)
{
    float4x4 gWorldViewProj[GRID_COUNT_X * GRID_COUNT_Y];
    float4 c[GRID_COUNT_X * GRID_COUNT_Y];
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PSInput VSMain(float4 position : POSITION, float4 color : COLOR, uint4 rindex : RINDEX)
{
    PSInput result;

    result.position = mul(position, gWorldViewProj[rindex[0]]);
    result.position.z = 0;
    result.color = color + c[rindex[0]];

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return input.color;
}
