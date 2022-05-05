FROM readyos:latest

# fail if build with libvpx > 1.5

ENV NASM_VER="2.13.01" \
 MCU_CONF_OPTS="--enable-static --disable-v4l --disable-v4l2" \
 FFMPEG_VER="4.3" \
 LIBVPX_VER="1.5.0" \
 MY_USER="mcu"

#WORKDIR /tmp/build/
COPY . /tmp/build/qazmeet
#RUN yum -y update && yum -y upgrade
RUN yum -y  install automake
#RUN cd /tmp/build/qazmeet/
RUN chmod -R +x /tmp/build/qazmeet/

RUN cd /tmp/build/qazmeet/ \
 && ./autogen.sh \
 && ./configure $MCU_CONF_OPTS
RUN cd /tmp/build/qazmeet/ \
 && make -j 3 \
 && make install
#RUN rm -rf /tmp/* \
# && yum remove -y wget bzip2 which git freetype-devel flex bison autoconf automake pkgconfi>
# rpm-build libtool libtool-ltdl-devel openssl-devel \
# && yum clean all || echo "not all clean" \
# && rm -rf /var/cache/yum

 EXPOSE 5060 5061 1420 1554 1720

CMD ["/opt/qazmeet/bin/qazmeet", "-x"]
 
