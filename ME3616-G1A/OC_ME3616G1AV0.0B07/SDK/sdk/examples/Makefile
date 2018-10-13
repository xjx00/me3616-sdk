################################################################################
##
## File   : Makefile
##
## Copyright (C) 2013-2018 Gosuncn. All rights reserved.
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
##      http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.
##
## Author : lixingyuan@gosuncn.cn
##
##  $Date: 2018/02/08 08:45:36GMT+08:00 $
##
################################################################################

FULL_DIRS := $(filter %/, $(wildcard ./*/))
SUB_DIRS := $(FULL_DIRS:%/=%)
SUB_DIRS_CLAEN := $(addsuffix _clean, $(SUB_DIRS))

all : $(SUB_DIRS)

.PHONY: clean
clean: $(SUB_DIRS_CLAEN)

.PHONY: $(SUB_DIRS)
$(SUB_DIRS):
	$(MAKE) -C $@
	
.PHONY: $(SUB_DIRS_CLAEN)
$(SUB_DIRS_CLAEN):
	$(MAKE) -C $(patsubst %_clean,%,$@) clean

