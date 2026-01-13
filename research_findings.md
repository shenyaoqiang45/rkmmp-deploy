# RKMPP MJPEG编解码研究发现

## 1. MPP库基本信息

### 库的功能
- **MPP (Media Process Platform)** 是瑞芯微提供的硬件编解码库
- 为用户空间屏蔽了复杂的底层操作
- 通过MPI接口提供媒体处理功能

### 支持的编解码格式
**视频解码支持:**
- H.265 / H.264 / H.263 / VP9 / VP8 / MPEG-4 / MPEG-2 / MPEG-1 / VC1 / MJPEG

**视频编码支持:**
- H.264 / VP8 / MJPEG

**视频处理:**
- 视频拷贝、缩放、色彩空间转换、场视频解交织

## 2. 核心数据结构

### MppPacket 和 MppFrame
- **MppPacket**: 一维缓存封装，表示码流数据
- **MppFrame**: 二维帧数据封装，表示图像数据
- 可以从MppMem和MppBuffer生成

### 编解码流程
1. 创建MppCtx实例
2. 初始化编解码器
3. 配置编解码参数
4. 循环进行put_packet/get_frame操作

## 3. 编码流程关键步骤
1. mpp_create - 创建编码器实例
2. mpp_init - 初始化编码器
3. mpp_enc_cfg_init - 设置编码配置
4. mpi->control - 配置参数
5. 循环: 
   - mpi->encode_put_frame - 输入YUV帧
   - mpi->encode_get_packet - 获取编码后的码流

## 4. 解码流程关键步骤
1. mpp_create - 创建解码器实例
2. mpp_init - 初始化解码器
3. 循环:
   - mpi->decode_put_packet - 输入码流
   - mpi->decode_get_frame - 获取解码后的YUV帧

## 5. 官方测试工具
- **mpi_enc_test**: 编码测试程序
- **mpi_dec_test**: 解码测试程序
- **mpi_dec_mt_test**: 多线程解码
- **mpi_dec_multi_test**: 多实例解码

## 6. NV12格式
- 支持NV12作为输入格式进行编码
- NV12是YUV 4:2:0格式，Y分量占4/6，UV分量各占1/6

## 7. MJPEG特性
- MJPEG是Motion JPEG，每帧都是独立的JPEG图像
- 适合于实时视频传输
- 支持硬件编解码

## 8. 关键配置参数
- **MPPEncRcCfg**: 码率控制方式
- **MPP_ENC_SET_CFG/MPP_ENC_GET_CFG**: 编码器参数配置命令
- **rc_mode**: 码率控制模式（如fix_qp）

## 9. 参考资源
- GitHub: https://github.com/vicharak-in/rockchip-linux-mpp (官方版本被DMCA下架)
- 文档: MPP Development Reference
- 示例代码: mpi_enc_test.c, mpi_dec_test.c

## 10. 实现策略
1. 创建C库接口，封装MPP底层API
2. 实现NV12→MJPEG编码函数
3. 实现MJPEG→NV12解码函数
4. 提供初始化、释放、参数配置接口
5. 编写完整的测试用例
