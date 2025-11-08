// Tencent is pleased to support the open source community by making ncnn available.
//
// Copyright (C) 2021 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
// https://opensource.org/licenses/BSD-3-Clause
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

package com.tencent.scrfdncnn;

public class SCRFDNcnn
{


    public native void init( String scrfdParamPath, String scrfdBinPath,String featParamPath, String featBinPath);


    //返回向量值
    public native float[][]  getEmbeddingFromImage(String imagePath);

    //返回相似度
    public native float compareEmbeddingWithImage(float[][] embd,String imagePath);



    static {
        System.loadLibrary("scrfdncnn");
    }
}
