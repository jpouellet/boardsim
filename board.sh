#!/bin/sh

set -e

top="$1"
shift

inst="$1"
shift

awk 'BEGIN{top=ARGV[2]; delete ARGV[2]} {print $0} /TOPMODULE/{print top}' DE2_top.v "$top $top ($inst);" > top.v
iverilog -o "${top}.vvp" top.v "$@"
vvp -M. -mDE2 "${top}.vvp"
