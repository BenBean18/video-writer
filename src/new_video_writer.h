#include <iostream>
#include <stdio.h>
#include <stdlib.h>

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <inttypes.h>

    #include "libavutil/frame.h"
    #include "libavutil/imgutils.h"

}

class VideoWriter {
public:
    VideoWriter(char* filename, uint16_t fps, int fwidth, int fheight, bool iscolor);
    ~VideoWriter();
    
    bool write(uint8_t* data); //assumed data is given in opencv bgr format
    //there is no need to release the object. It is done automatically

    int flush_encoder(AVCodecContext* c, unsigned int stream_index);
private:
    AVFrame* frame;
    AVPacket* pkt;
    AVFormatContext* fmt_ctx;
    AVCodecContext* c;
    AVStream* stream;
    uint64_t frame_count;
};