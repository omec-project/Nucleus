
# Copyright 2019-present Open Networking Foundation
#
# SPDX-License-Identifier: Apache-2.0
#

ARG BASE_OS=ubuntu:16.04

FROM $BASE_OS AS builder
WORKDIR /openmme
COPY patches ./tmp/patches
COPY install_builddeps.sh .
RUN ./install_builddeps.sh
COPY Makefile Makefile.common ./
COPY include/cmn ./include/cmn
COPY include/common ./include/common
COPY include/stateMachineFwk ./include/stateMachineFwk
COPY include/mme-app ./include/mme-app
COPY src/cmn ./src/cmn
COPY src/common ./src/common
COPY src/gtpV2Codec ./src/gtpV2Codec
COPY src/mmeGrpcClient ./src/mmeGrpcClient
COPY src/mmeGrpcProtos ./src/mmeGrpcProtos
COPY src/stateMachineFwk ./src/stateMachineFwk
COPY src/s1ap/asn1c ./src/s1ap/asn1c
RUN make -C src/cmn
RUN make -C src/common
RUN make -C src/gtpV2Codec
RUN make -C src/stateMachineFwk
RUN make -C src/mmeGrpcProtos
RUN make -C src/mmeGrpcClient
RUN make -C src/cmn
COPY include/s11 ./include/s11
COPY include/s6a ./include/s6a
COPY include/s1ap ./include/s1ap
COPY src/s11 ./src/s11
RUN make -C src/s11/cpp_utils --debug
RUN make -C src/s11
COPY src/s6a ./src/s6a
RUN make -C src/s6a
COPY src/mme-app ./src/mme-app
RUN make -C src/mme-app
COPY src/s1ap ./src/s1ap
RUN make -C src/s1ap/s1apContextManager
RUN make -C src/s1ap

RUN make install

FROM $BASE_OS AS runtime
COPY install_rundeps.sh .
RUN bash -c "source ./install_rundeps.sh && install_run_deps && install_run_utils && cleanup_image"
COPY --from=builder /usr/local/lib /usr/local/lib
COPY --from=builder /usr/lib/x86_64-linux-gnu /usr/local/lib
COPY --from=builder /tmp/grpc/libs /usr/lib/grpc
COPY --from=builder /tmp/prometheus/_build/deploy/usr/local/lib /usr/local/lib

WORKDIR /openmme/target
COPY --from=builder /openmme/target .

ARG org_label_schema_version=unknown
ARG org_label_schema_vcs_url=unknown
ARG org_label_schema_vcs_ref=unknown
ARG org_label_schema_build_date=unknown
ARG org_opencord_vcs_commit_date=unknown

LABEL org.label-schema.schema-version=1.0 \
      org.label-schema.name=openmme \
      org.label-schema.version=$org_label_schema_version \
      org.label-schema.vcs-url=$org_label_schema_vcs_url \
      org.label-schema.vcs-ref=$org_label_schema_vcs_ref \
      org.label-schema.build-date=$org_label_schema_build_date \
      org.opencord.vcs-commit-date=$org_opencord_vcs_commit_date
