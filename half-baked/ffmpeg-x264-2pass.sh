#!/bin/bash

set -u
set -e

if [ $# -lt 4 ]
then
	echo "$0 <input.avi> <video-bitrate> <audio-bitrate> <output.mp4> [<ffmpeg-options>]" >&2
	exit 1
fi

acodec=aac
#acodec=libvo_aacenc

input=$1
shift
vbr=${1}k
shift
abr=${1}
shift
out=$1
shift

declare opts=()
#opts=(-i "$input" -c:v libx264 -preset medium -b:v "$vbr" -c:a copy -f mp4)
#opts=(-i "$input" -c:v libx264 -preset medium -b:v "$vbr" -c:a $acodec -b:a "$abr" -f mp4)
opts+=(-i "$input" -c:v libx264 -preset medium -b:v "$vbr" -c:a $acodec)
if [ "${abr}" != '' ]; then opts+=(-b:a "$abr"k); fi
opts+=(-f mp4)


#FFMPEG=${FFMPEG:-ffmpeg3.2.4}
#FFMPEG=ffmpeg2.6
FFMPEG=ffmpeg
#FFMPEG="qemu-i386 /srv/bin/ffmpeg2.6"

$FFMPEG "${opts[@]}" -x264-params pass=1 "$@" -y /dev/null

$FFMPEG "${opts[@]}" -x264-params pass=2 "$@" "$out"
