# Copyright 2019-present Open Networking Foundation
# Copyright 2019-present, Infosys Limited.
# Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
# Copyright (c) 2017 Intel Corporation
#
# SPDX-License-Identifier: Apache-2.0
#

PROJECT_NAME		 := nucleus
VERSION                  ?= $(shell cat ./VERSION)

TOPTARGETS 		 := all clean install

SUBDIRS 		 := src/cmn \
					src/common \
					src/gtpV2Codec \
					src/stateMachineFwk \
					src/mmeGrpcProtos \
					src/mmeGrpcClient \
					src/mme-app \
					src/s1ap/s1apContextManager \
					src/s1ap \
					src/s11/cpp_utils/ \
					src/s11 \
					src/s10/cpp_utils/ \
					src/s10 \
					src/s6a

#$(wildcard src/mme-app/.)

$(TOPTARGETS): $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

## Docker related
DOCKER_REGISTRY          ?=
DOCKER_REPOSITORY        ?=
DOCKER_BUILD_ARGS        ?=
DOCKER_TAG               ?= ${VERSION}
DOCKER_IMAGENAME         := ${DOCKER_REGISTRY}${DOCKER_REPOSITORY}${PROJECT_NAME}:${DOCKER_TAG}

## Docker labels. Only set ref and commit date if committed
DOCKER_LABEL_VCS_URL     ?= $(shell git remote get-url $(shell git remote))
DOCKER_LABEL_VCS_REF     ?= $(shell git diff-index --quiet HEAD -- && git rev-parse HEAD || echo "unknown")
DOCKER_LABEL_COMMIT_DATE ?= $(shell git diff-index --quiet HEAD -- && git show -s --format=%cd --date=iso-strict HEAD || echo "unknown" )
DOCKER_LABEL_BUILD_DATE  ?= $(shell date -u "+%Y-%m-%dT%H:%M:%SZ")

# https://docs.docker.com/engine/reference/commandline/build/#specifying-target-build-stage---target
docker-build:
	docker build $(DOCKER_BUILD_ARGS) \
		-t ${DOCKER_IMAGENAME} \
		--build-arg org_label_schema_version="${VERSION}" \
		--build-arg org_label_schema_vcs_url="${DOCKER_LABEL_VCS_URL}" \
		--build-arg org_label_schema_vcs_ref="${DOCKER_LABEL_VCS_REF}" \
		--build-arg org_label_schema_build_date="${DOCKER_LABEL_BUILD_DATE}" \
		--build-arg org_opencord_vcs_commit_date="${DOCKER_LABEL_COMMIT_DATE}" \
		.

docker-push:
	docker push ${DOCKER_IMAGENAME}

.PHONY: $(TOPTARGETS) $(SUBDIRS) docker-build docker-push

cppcheck:
	cppcheck --error-exitcode=1 -q .
