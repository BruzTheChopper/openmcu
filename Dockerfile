FROM readyos:latest

# fail if build with libvpx > 1.5

ENV NASM_VER="2.13.01" \
 MCU_CONF_OPTS="--enable-static --disable-v4l --disable-v4l2" \
 FFMPEG_VER="4.3" \
 LIBVPX_VER="1.5.0" \
 MY_USER="mcu"

#WORKDIR /tmp/build/
COPY . /tmp/build/qazmeet
RUN ls -l /tmp/build/
RUN cd /tmp/build/qazmeet/ \  
 && ./autogen.sh && ./configure $MCU_CONF_OPTS 
RUN cd /tmp/build/qazmeet/ && make -j 3 && make install 
RUN rm -rf /tmp/* \
 && yum remove -y wget bzip2 which git freetype-devel flex bison autoconf automake pkgconfig \
 rpm-build libtool libtool-ltdl-devel openssl-devel \
 && yum clean all || echo "not all clean" \
 && rm -rf /var/cache/yum
 
 EXPOSE 5060 5061 1420 1554 1720

CMD ["/opt/qazmeet/bin/qazmeet", "-x"]
 