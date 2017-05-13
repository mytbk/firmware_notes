// EdkCompatibilityPkg/Foundation/Framework/Include/PeiApi.h

struct _EFI_PEI_SERVICES {
  EFI_TABLE_HEADER              Hdr;

  //
  // PPI Functions
  //
	EFI_PEI_INSTALL_PPI           InstallPpi; // 0x18
	EFI_PEI_REINSTALL_PPI         ReInstallPpi; // 0x1c
	EFI_PEI_LOCATE_PPI            LocatePpi; // 0x20
	EFI_PEI_NOTIFY_PPI            NotifyPpi; // 0x24

  //
  // Boot Mode Functions
  //
	EFI_PEI_GET_BOOT_MODE         GetBootMode; // 0x28
	EFI_PEI_SET_BOOT_MODE         SetBootMode; // 0x2c

  //
  // HOB Functions
  //
	EFI_PEI_GET_HOB_LIST          GetHobList; // 0x30
	EFI_PEI_CREATE_HOB            CreateHob; // 0x34

  //
  // Firmware Volume Functions
  //
#if (PI_SPECIFICATION_VERSION < 0x00010000)
  EFI_PEI_FFS_FIND_NEXT_VOLUME  FfsFindNextVolume;
  EFI_PEI_FFS_FIND_NEXT_FILE    FfsFindNextFile;
  EFI_PEI_FFS_FIND_SECTION_DATA FfsFindSectionData;
#else
  EFI_PEI_FFS_FIND_NEXT_VOLUME2  FfsFindNextVolume;
  EFI_PEI_FFS_FIND_NEXT_FILE2    FfsFindNextFile;
  EFI_PEI_FFS_FIND_SECTION_DATA2 FfsFindSectionData;
#endif
  //
  // PEI Memory Functions
  //
	EFI_PEI_INSTALL_PEI_MEMORY    InstallPeiMemory; // 0x44
	EFI_PEI_ALLOCATE_PAGES        AllocatePages; // 0x48
	EFI_PEI_ALLOCATE_POOL         AllocatePool; // 0x4c
	EFI_PEI_COPY_MEM              CopyMem; // 0x50
	EFI_PEI_SET_MEM               SetMem; // 0x54

  //
  // Status Code
  //
	EFI_PEI_REPORT_STATUS_CODE    PeiReportStatusCode; // 0x58

  //
  // Reset
  //
	EFI_PEI_RESET_SYSTEM          PeiResetSystem; // 0x5c

  //
  // Pointer to PPI interface
  //
#if (PI_SPECIFICATION_VERSION < 0x00010000)

  PEI_CPU_IO_PPI                 *CpuIo;
#if defined (SUPPORT_DEPRECATED_PCI_CFG_PPI)
  PEI_PCI_CFG_PPI                *PciCfg;
#else
  ECP_PEI_PCI_CFG_PPI            *PciCfg;
#endif
#else
	EFI_PEI_CPU_IO_PPI             *CpuIo; // 0x60
	EFI_PEI_PCI_CFG2_PPI           *PciCfg; // 0x64

  //
  // New interfaceas added by the PI 1.0
  //
  EFI_PEI_FFS_FIND_BY_NAME        FfsFindFileByName;
  EFI_PEI_FFS_GET_FILE_INFO       FfsGetFileInfo;
  EFI_PEI_FFS_GET_VOLUME_INFO     FfsGetVolumeInfo;
  EFI_PEI_REGISTER_FOR_SHADOW     RegisterForShadow;
#endif

};
