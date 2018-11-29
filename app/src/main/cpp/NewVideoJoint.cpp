//
// Created by Administrator on 2018/11/29/029.
//

/**
 * 新合并视频的方式
 * 其他视频文件-> video.ts(这里统一好视频的分辨率)
 *
 * video1.ts |
 *           | -> concat -> out.mp4
 * video2.ts |
 *
 *
 * http://bbs.chinaffmpeg.com/forum.php?mod=viewthread&tid=356&highlight=%E8%A7%86%E9%A2%91%E5%90%88%E5%B9%B6
 *
 *  ffmpeg -i v1080.mp4 -vf scale=640:360 vbaobao640_360.mp4 -hide_banner 转分辨率
 *
 *  ffmpeg -i test.mp4 -vcodec copy -acodec copy -vbsf h264_mp4toannexb test.ts 转ts
 */

#include "NewVideoJoint.h"

extern "C"{
//#include "concatdec.h"
}

NewVideoJoint::NewVideoJoint(){
//    ConcatFile  cf ;
}
NewVideoJoint::~NewVideoJoint(){

}
