/*
 * Copyright (c) 2019-2021, NVIDIA CORPORATION. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.

 * Edited by Marcos Luciano
 * https://www.github.com/marcoslucianops
 */

#ifndef __YOLO_PLUGINS__
#define __YOLO_PLUGINS__

#include <cassert>
#include <cstring>
#include <cuda_runtime_api.h>
#include <iostream>
#include <memory>

#include <vector>

#include "NvInferPlugin.h"

#define CHECK(status)                                                                              \
    {                                                                                              \
        if (status != 0)                                                                           \
        {                                                                                          \
            std::cout << "CUDA failure: " << cudaGetErrorString(status) << " in file " << __FILE__ \
                      << " at line " << __LINE__ << std::endl;                                     \
            abort();                                                                               \
        }                                                                                          \
    }

namespace
{
    const char *YOLOLAYER_PLUGIN_VERSION{"1"};
    const char *YOLOLAYER_PLUGIN_NAME{"YoloLayer_TRT"};
} // namespace

class YoloLayer : public nvinfer1::IPluginV2
{
public:
    YoloLayer(const void *data, size_t length);
    YoloLayer(const uint &numBoxes, const uint &numClasses, const uint &gridSizeX, const uint &gridSizeY,
              const uint model_type, const uint new_coords, const float scale_x_y, const float beta_nms,
              const std::vector<float> anchors, const std::vector<std::vector<int>> mask);
    const char *getPluginType() const noexcept override { return YOLOLAYER_PLUGIN_NAME; }
    const char *getPluginVersion() const noexcept override { return YOLOLAYER_PLUGIN_VERSION; }
    int getNbOutputs() const noexcept override { return 1; }

    nvinfer1::Dims getOutputDimensions(
        int index, const nvinfer1::Dims *inputs,
        int nbInputDims) noexcept override;

    bool supportsFormat(
        nvinfer1::DataType type, nvinfer1::PluginFormat format) const noexcept override;

    void configureWithFormat(
        const nvinfer1::Dims *inputDims, int nbInputs,
        const nvinfer1::Dims *outputDims, int nbOutputs,
        nvinfer1::DataType type, nvinfer1::PluginFormat format, int maxBatchSize) noexcept override;

    int initialize() noexcept override { return 0; }
    void terminate() noexcept override {}
    size_t getWorkspaceSize(int maxBatchSize) const noexcept override { return 0; }
    int enqueue(
        int batchSize, void const *const *inputs, void *const *outputs,
        void *workspace, cudaStream_t stream) noexcept override;
    size_t getSerializationSize() const noexcept override;
    void serialize(void *buffer) const noexcept override;
    void destroy() noexcept override { delete this; }
    nvinfer1::IPluginV2 *clone() const noexcept override;

    void setPluginNamespace(const char *pluginNamespace) noexcept override
    {
        m_Namespace = pluginNamespace;
    }
    virtual const char *getPluginNamespace() const noexcept override
    {
        return m_Namespace.c_str();
    }

private:
    uint m_NumBoxes{0};
    uint m_NumClasses{0};
    uint m_GridSizeX{0};
    uint m_GridSizeY{0};
    uint64_t m_OutputSize{0};
    std::string m_Namespace{""};

    uint m_type{0};
    uint m_new_coords{0};
    float m_scale_x_y{0};
    float m_beta_nms{0};
    std::vector<float> m_Anchors;
    std::vector<std::vector<int>> m_Mask;
};

class YoloLayerPluginCreator : public nvinfer1::IPluginCreator
{
public:
    YoloLayerPluginCreator() {}
    ~YoloLayerPluginCreator() {}

    const char *getPluginName() const noexcept override { return YOLOLAYER_PLUGIN_NAME; }
    const char *getPluginVersion() const noexcept override { return YOLOLAYER_PLUGIN_VERSION; }

    const nvinfer1::PluginFieldCollection *getFieldNames() noexcept override
    {
        std::cerr << "YoloLayerPluginCreator::getFieldNames is not implemented" << std::endl;
        return nullptr;
    }

    nvinfer1::IPluginV2 *createPlugin(
        const char *name, const nvinfer1::PluginFieldCollection *fc) noexcept override
    {
        std::cerr << "YoloLayerPluginCreator::getFieldNames is not implemented";
        return nullptr;
    }

    nvinfer1::IPluginV2 *deserializePlugin(
        const char *name, const void *serialData, size_t serialLength) noexcept override
    {
        std::cout << "Deserialize yoloLayer plugin: " << name << std::endl;
        return new YoloLayer(serialData, serialLength);
    }

    void setPluginNamespace(const char *libNamespace) noexcept override
    {
        m_Namespace = libNamespace;
    }
    const char *getPluginNamespace() const noexcept override
    {
        return m_Namespace.c_str();
    }

private:
    std::string m_Namespace{""};
};

extern int kNUM_CLASSES;
extern float kBETA_NMS;
extern std::vector<float> kANCHORS;
extern std::vector<std::vector<int>> kMASK;

#endif // __YOLO_PLUGINS__
