FROM archlinux:base-devel
RUN pacman -Syu --noconfirm
RUN pacman -S parallel git github-cli cmake vi vim kakoune emacs nano --noconfirm
RUN git clone https://github.com/DocSunset/lili.git
RUN cd lili && make && sudo make install
RUN rm -rf lili
# docker image build -t sygaldry .
# # change the argument to the `--device` flag to your serial port
# docker run --interactive --tty --device=/dev/ttyUSB0 sygaldry 
# gh auth login
# gh repo clone DocSunset/sygaldry -- --recurse-submodules
