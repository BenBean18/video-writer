#include "new_video_writer.h"

VideoWriter::VideoWriter(char* filename, uint16_t fps, int fwidth, int fheight, bool iscolor=true) {

    // Register all formats and codecs
    av_register_all();

    // Get the codec
    AVCodec* codec = avcodec_find_encoder_by_name("h264_v4l2m2m");
    if (!codec) {
        std::cout << "Codec not found" << std::endl;
        exit(1);
    }

    // Allocate context
    c = avcodec_alloc_context3(codec);
    if (!c) {
        std::cout << "Could not allocate video codec context" << std::endl;
        exit(1);
    }

    fmt_ctx = avformat_alloc_context();
    if (!fmt_ctx) {
        std::cout << "Could not allocate video format context" << std::endl;
        exit(1);
    }

    // Find the output format
    AVOutputFormat *fmt = av_guess_format(NULL, filename, NULL);
    if (!fmt) {
        std::cout << "Could not find output format" << std::endl;
        exit(1);
    }

    // Set the output format
    fmt_ctx->oformat = fmt;
    fmt_ctx->oformat->video_codec = codec->id;

    // Setup format context IO
    if (avio_open(&fmt_ctx->pb, filename, AVIO_FLAG_WRITE) < 0) {
        std::cout << "Could not open output file" << std::endl;
        exit(1);
    }

    // Set the parameters
    c->bit_rate = 5000000;
    c->width = fwidth;
    c->height = fheight;
    c->time_base = (AVRational){1, fps};
    c->gop_size = 60;
    // c->max_b_frames = 1;
    // use bgr24
    c->pix_fmt = AV_PIX_FMT_RGB24;

    // Open the codec
    if (avcodec_open2(c, codec, NULL) < 0) {
        std::cout << "Could not open codec" << std::endl;
        exit(1);
    }

    // Set up stream
    stream = avformat_new_stream(fmt_ctx, codec);
    if (!stream) {
        std::cout << "Could not allocate stream" << std::endl;
        exit(1);
    }
    stream->id = fmt_ctx->nb_streams - 1;

    stream->time_base = (AVRational){1, fps};
    stream->avg_frame_rate = (AVRational){fps, 1};

    // Set dimensions in stream
    AVCodecParameters* codecpar = stream->codecpar;
    codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
    codecpar->codec_id = codec->id;
    codecpar->width = fwidth;
    codecpar->height = fheight;
    codecpar->format = c->pix_fmt;

    // Allocate frame
    frame = av_frame_alloc();
    if (!frame) {
        std::cout << "Could not allocate video frame" << std::endl;
        exit(1);
    }
    frame->format = c->pix_fmt;
    frame->width = c->width;
    frame->height = c->height;

    // Allocate an image buffer
    int ret = av_image_alloc(frame->data, frame->linesize, c->width, c->height, c->pix_fmt, 32);
    if (ret < 0) {
        std::cout << "Could not allocate raw picture buffer" << std::endl;
        exit(1);
    }

    // Allocate packet
    pkt = av_packet_alloc();
    if (!pkt) {
        std::cout << "Could not allocate packet" << std::endl;
        exit(1);
    }

    // Initialize the packet
    av_init_packet(pkt);
    pkt->data = NULL;
    pkt->size = 0;

    frame_count = 0;

    std::cout << "packet initialized" << std::endl;

    // Write the header
    ret = avformat_write_header(fmt_ctx, NULL);
    if (ret < 0) {
        std::cout << "Error occurred when opening output file" << std::endl;
        exit(1);
    }
}

VideoWriter::~VideoWriter() {
    // Flush the encoder
    int ret = flush_encoder(c, 0);
    if (ret < 0) {
        std::cout << "Flushing encoder failed" << std::endl;
        exit(1);
    }

    // Write trailer
    av_write_trailer(fmt_ctx);

    // Clean
    if (c) {
        avcodec_free_context(&c);
    }
    if (frame) {
        av_frame_free(&frame);
    }
    if (pkt) {
        av_packet_free(&pkt);
    }
    if (fmt_ctx) {
        avio_closep(&fmt_ctx->pb);
        avformat_free_context(fmt_ctx);
    }
}

bool VideoWriter::write(uint8_t* data) {
    std::cout << "frame number " << std::to_string(frame_count) << std::endl;
    printf("writing to %p", data);
    // Store data in frame
    int ret = av_image_fill_arrays(frame->data, frame->linesize, data, c->pix_fmt, c->width, c->height, 1);
    if (ret < 0) {
        std::cout << "Could not fill image frame" << std::endl;
        exit(1);
    }

    ret = avcodec_send_frame(c, frame);
    if (ret < 0) {
        std::cout << "Error sending a frame for encoding" << std::endl;
        exit(1);
    }

    while (ret >= 0) {
        ret = avcodec_receive_packet(c, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return false;
        } else if (ret < 0) {
            std::cout << "Error during encoding" << std::endl;
            exit(1);
        }

        // Set packet timestamp
        pkt->pts = pkt->dts = frame_count * 1000 / c->time_base.num;

        // Increment frame count
        frame_count++;

        // write using fmt_ctx
        av_write_frame(fmt_ctx, pkt);

        av_packet_unref(pkt);
    }

    return true;
}

int VideoWriter::flush_encoder(AVCodecContext* c, unsigned int stream_index) {
    // int ret;
    // int got_frame;
    // AVPacket enc_pkt;
    // if (!(c->codec->capabilities & AV_CODEC_CAP_DELAY)) {
    //     return 0;
    // }
    // while (1) {
    //     enc_pkt.data = NULL;
    //     enc_pkt.size = 0;
    //     av_init_packet(&enc_pkt);
    //     ret = avcodec_encode_video2(c, &enc_pkt, NULL, &got_frame);
    //     av_frame_free(NULL);
    //     if (ret < 0) {
    //         break;
    //     }
    //     if (!got_frame) {
    //         ret = 0;
    //         break;
    //     }
    // }
    // return ret;
    return 0;
}