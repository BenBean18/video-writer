// Signatures for the class from new_video_writer.cpp

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
};