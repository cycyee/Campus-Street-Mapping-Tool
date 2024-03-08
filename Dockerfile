FROM python:3.10

RUN apt-get update && \
    apt-get install build-essential libgtest-dev libgmock-dev less vim sudo -y --no-install-recommends && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/\* /tmp/\* /var/tmp/* && \
    useradd -m docker 

WORKDIR /workspace

CMD ["/bin/bash"]