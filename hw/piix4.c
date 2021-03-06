/*
 * QEMU PIIX4 PCI Bridge Emulation
 *
 * Copyright (c) 2006 Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "hw.h"
#include "pc.h"
#include "pci.h"
#include "isa.h"
#include "sysbus.h"

PCIDevice *piix4_dev;

typedef struct PIIX4State {
    PCIDevice dev;
} PIIX4State;

static void piix4_reset(void *opaque)
{
    PIIX4State *d = opaque;
    uint8_t *pci_conf = d->dev.config;

    pci_conf[0x04] = 0x07; // master, memory and I/O
    pci_conf[0x05] = 0x00;
    pci_conf[0x06] = 0x00;
    pci_conf[0x07] = 0x02; // PCI_status_devsel_medium
    pci_conf[0x4c] = 0x4d;
    pci_conf[0x4e] = 0x03;
    pci_conf[0x4f] = 0x00;
    pci_conf[0x60] = 0x0a; // PCI A -> IRQ 10
    pci_conf[0x61] = 0x0a; // PCI B -> IRQ 10
    pci_conf[0x62] = 0x0b; // PCI C -> IRQ 11
    pci_conf[0x63] = 0x0b; // PCI D -> IRQ 11
    pci_conf[0x69] = 0x02;
    pci_conf[0x70] = 0x80;
    pci_conf[0x76] = 0x0c;
    pci_conf[0x77] = 0x0c;
    pci_conf[0x78] = 0x02;
    pci_conf[0x79] = 0x00;
    pci_conf[0x80] = 0x00;
    pci_conf[0x82] = 0x00;
    pci_conf[0xa0] = 0x08;
    pci_conf[0xa2] = 0x00;
    pci_conf[0xa3] = 0x00;
    pci_conf[0xa4] = 0x00;
    pci_conf[0xa5] = 0x00;
    pci_conf[0xa6] = 0x00;
    pci_conf[0xa7] = 0x00;
    pci_conf[0xa8] = 0x0f;
    pci_conf[0xaa] = 0x00;
    pci_conf[0xab] = 0x00;
    pci_conf[0xac] = 0x00;
    pci_conf[0xae] = 0x00;
}

static const VMStateDescription vmstate_piix4 = {
    .name = "PIIX4",
    .version_id = 2,
    .minimum_version_id = 2,
    .minimum_version_id_old = 2,
    .fields      = (VMStateField[]) {
        VMSTATE_PCI_DEVICE(dev, PIIX4State),
        VMSTATE_END_OF_LIST()
    }
};

static int piix4_initfn(PCIDevice *dev)
{
    PIIX4State *d = DO_UPCAST(PIIX4State, dev, dev);

    isa_bus_new(&d->dev.qdev);
    piix4_dev = &d->dev;
    qemu_register_reset(piix4_reset, d);
    return 0;
}

int piix4_init(PCIBus *bus, int devfn)
{
    PCIDevice *d;

    d = pci_create_simple_multifunction(bus, devfn, true, "PIIX4");
    return d->devfn;
}

static PCIDeviceInfo piix4_info[] = {
    {
        .qdev.name    = "PIIX4",
        .qdev.desc    = "ISA bridge",
        .qdev.size    = sizeof(PIIX4State),
        .qdev.vmsd    = &vmstate_piix4,
        .qdev.no_user = 1,
        .no_hotplug   = 1,
        .init         = piix4_initfn,
        .vendor_id    = PCI_VENDOR_ID_INTEL,
        .device_id    = PCI_DEVICE_ID_INTEL_82371AB_0, // 82371AB/EB/MB PIIX4 PCI-to-ISA bridge
        .class_id     = PCI_CLASS_BRIDGE_ISA,
    },{
        /* end of list */
    }
};

static void piix4_register(void)
{
    pci_qdev_register_many(piix4_info);
}
device_init(piix4_register);
