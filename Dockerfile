FROM archlinux:base-devel
RUN pacman -Syu --noconfirm
RUN pacman -S parallel git github-cli cmake vi vim kakoune emacs nano --noconfirm
RUN git clone https://github.com/DocSunset/lili.git
RUN cd lili && make && sudo make install
RUN rm -rf lili
# docker image build -t sygaldry .
# docker run --interactive --tty sygaldry
# gh auth login
# gh repo clone DocSunset/sygaldry -- --recurse-submodules
