#ifndef __ACC_DOCKER_PCI_LOOKUP__
#define __ACC_DOCKER_PCI_LOOKUP__

#include <pci/pci.h> // libpci 이용! (Compile시 libpci-dev 패키지 설치해야 함) - Compile 'gcc pci-lookup.c -lpci -o pci-lookup'
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "acc-manager.hpp"

#endif
