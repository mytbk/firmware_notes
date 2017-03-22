/* MdePkg/Include/Uefi/UefiMultiPhase.h */

///
/// Data structure that precedes all of the standard EFI table types.
///
typedef struct {
  ///
  /// A 64-bit signature that identifies the type of table that follows.
  /// Unique signatures have been generated for the EFI System Table,
  /// the EFI Boot Services Table, and the EFI Runtime Services Table.
  ///
  UINT64  Signature;
  ///
  /// The revision of the EFI Specification to which this table
  /// conforms. The upper 16 bits of this field contain the major
  /// revision value, and the lower 16 bits contain the minor revision
  /// value. The minor revision values are limited to the range of 00..99.
  ///
  UINT32  Revision;
  ///
  /// The size, in bytes, of the entire table including the EFI_TABLE_HEADER.
  ///
  UINT32  HeaderSize;
  ///
  /// The 32-bit CRC for the entire table. This value is computed by
  /// setting this field to 0, and computing the 32-bit CRC for HeaderSize bytes.
  ///
  UINT32  CRC32;
  ///
  /// Reserved field that must be set to 0.
  ///
  UINT32  Reserved;
} EFI_TABLE_HEADER;


/* MdePkg/Include/Uefi/UefiSpec.h */

///
/// EFI Runtime Services Table.
///
typedef struct {
	///
	/// The table header for the EFI Runtime Services Table.
	///
	EFI_TABLE_HEADER                Hdr;

	//
	// Time Services
	//
	EFI_GET_TIME                    GetTime; // 0x18
	EFI_SET_TIME                    SetTime; // 0x20
	EFI_GET_WAKEUP_TIME             GetWakeupTime; // 0x28
	EFI_SET_WAKEUP_TIME             SetWakeupTime; // 0x30

	//
	// Virtual Memory Services
	//
	EFI_SET_VIRTUAL_ADDRESS_MAP     SetVirtualAddressMap; // 0x38
	EFI_CONVERT_POINTER             ConvertPointer; //0x40

	//
	// Variable Services
	//
	EFI_GET_VARIABLE                GetVariable; // 0x48
	EFI_GET_NEXT_VARIABLE_NAME      GetNextVariableName; // 0x50
	EFI_SET_VARIABLE                SetVariable; // 0x58

	//
	// Miscellaneous Services
	//
	EFI_GET_NEXT_HIGH_MONO_COUNT    GetNextHighMonotonicCount; // 0x60
	EFI_RESET_SYSTEM                ResetSystem; // 0x68

	//
	// UEFI 2.0 Capsule Services
	//
	EFI_UPDATE_CAPSULE              UpdateCapsule; // 0x70
	EFI_QUERY_CAPSULE_CAPABILITIES  QueryCapsuleCapabilities; // 0x78

	//
	// Miscellaneous UEFI 2.0 Service
	//
	EFI_QUERY_VARIABLE_INFO         QueryVariableInfo; // 0x80
} EFI_RUNTIME_SERVICES;


#define EFI_BOOT_SERVICES_SIGNATURE   SIGNATURE_64 ('B','O','O','T','S','E','R','V')
#define EFI_BOOT_SERVICES_REVISION    EFI_SPECIFICATION_VERSION

///
/// EFI Boot Services Table.
///
typedef struct {
	///
	/// The table header for the EFI Boot Services Table.
	///
	EFI_TABLE_HEADER                Hdr;

	//
	// Task Priority Services
	//
	EFI_RAISE_TPL                   RaiseTPL; // 0x18
	EFI_RESTORE_TPL                 RestoreTPL; // 0x20

	//
	// Memory Services
	//
	EFI_ALLOCATE_PAGES              AllocatePages; // 0x28
	EFI_FREE_PAGES                  FreePages; // 0x30
	EFI_GET_MEMORY_MAP              GetMemoryMap; // 0x38
	EFI_ALLOCATE_POOL               AllocatePool; // 0x40
	EFI_FREE_POOL                   FreePool; // 0x48

	//
	// Event & Timer Services
	//
	EFI_CREATE_EVENT                  CreateEvent; // 0x50
	EFI_SET_TIMER                     SetTimer; // 0x58
	EFI_WAIT_FOR_EVENT                WaitForEvent; // 0x60
	EFI_SIGNAL_EVENT                  SignalEvent; // 0x68
	EFI_CLOSE_EVENT                   CloseEvent; // 0x70
	EFI_CHECK_EVENT                   CheckEvent; // 0x78

	//
	// Protocol Handler Services
	//
	EFI_INSTALL_PROTOCOL_INTERFACE    InstallProtocolInterface; // 0x80
	EFI_REINSTALL_PROTOCOL_INTERFACE  ReinstallProtocolInterface; // 0x88
	EFI_UNINSTALL_PROTOCOL_INTERFACE  UninstallProtocolInterface; // 0x90
	EFI_HANDLE_PROTOCOL               HandleProtocol; // 0x98
	VOID                              *Reserved; // 0xa0
	EFI_REGISTER_PROTOCOL_NOTIFY      RegisterProtocolNotify; // 0xa8
	EFI_LOCATE_HANDLE                 LocateHandle; // 0xb0
	EFI_LOCATE_DEVICE_PATH            LocateDevicePath; // 0xb8
	EFI_INSTALL_CONFIGURATION_TABLE   InstallConfigurationTable; // 0xc0

	//
	// Image Services
	//
	EFI_IMAGE_LOAD                    LoadImage; // 0xc8
	EFI_IMAGE_START                   StartImage; // 0xd0
	EFI_EXIT                          Exit; // 0xd8
	EFI_IMAGE_UNLOAD                  UnloadImage; // 0xe0
	EFI_EXIT_BOOT_SERVICES            ExitBootServices; // 0xe8

	//
	// Miscellaneous Services
	//
	EFI_GET_NEXT_MONOTONIC_COUNT      GetNextMonotonicCount; // 0xf0
	EFI_STALL                         Stall; // 0xf8
	EFI_SET_WATCHDOG_TIMER            SetWatchdogTimer; // 0x100

	//
	// DriverSupport Services
	//
	EFI_CONNECT_CONTROLLER            ConnectController; // 0x108
	EFI_DISCONNECT_CONTROLLER         DisconnectController; // 0x110

	//
	// Open and Close Protocol Services
	//
	EFI_OPEN_PROTOCOL                 OpenProtocol; // 0x118
	EFI_CLOSE_PROTOCOL                CloseProtocol; // 0x120
	EFI_OPEN_PROTOCOL_INFORMATION     OpenProtocolInformation; // 0x128

	//
	// Library Services
	//
	EFI_PROTOCOLS_PER_HANDLE          ProtocolsPerHandle; // 0x130
	EFI_LOCATE_HANDLE_BUFFER          LocateHandleBuffer; // 0x138
	EFI_LOCATE_PROTOCOL               LocateProtocol; // 0x140
	EFI_INSTALL_MULTIPLE_PROTOCOL_INTERFACES    InstallMultipleProtocolInterfaces; // 0x148
	EFI_UNINSTALL_MULTIPLE_PROTOCOL_INTERFACES  UninstallMultipleProtocolInterfaces; // 0x150

	//
	// 32-bit CRC Services
	//
	EFI_CALCULATE_CRC32               CalculateCrc32; // 0x158

	//
	// Miscellaneous Services
	//
	EFI_COPY_MEM                      CopyMem; // 0x160
	EFI_SET_MEM                       SetMem; // 0x168
	EFI_CREATE_EVENT_EX               CreateEventEx; // 0x170
} EFI_BOOT_SERVICES;

///
/// Contains a set of GUID/pointer pairs comprised of the ConfigurationTable field in the
/// EFI System Table.
///
typedef struct {
  ///
  /// The 128-bit GUID value that uniquely identifies the system configuration table.
  ///
  EFI_GUID                          VendorGuid;
  ///
  /// A pointer to the table associated with VendorGuid.
  ///
  VOID                              *VendorTable;
} EFI_CONFIGURATION_TABLE;

///
/// EFI System Table
///
typedef struct {
	///
	/// The table header for the EFI System Table.
	///
	EFI_TABLE_HEADER                  Hdr;
	///
	/// A pointer to a null terminated string that identifies the vendor
	/// that produces the system firmware for the platform.
	///
	CHAR16                            *FirmwareVendor; // 0x18
	///
	/// A firmware vendor specific value that identifies the revision
	/// of the system firmware for the platform.
	///
	UINT32                            FirmwareRevision; // 0x20
	///
	/// The handle for the active console input device. This handle must support
	/// EFI_SIMPLE_TEXT_INPUT_PROTOCOL and EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL.
	///
	EFI_HANDLE                        ConsoleInHandle; // 0x28
	///
	/// A pointer to the EFI_SIMPLE_TEXT_INPUT_PROTOCOL interface that is
	/// associated with ConsoleInHandle.
	///
	EFI_SIMPLE_TEXT_INPUT_PROTOCOL    *ConIn; // 0x30
	///
	/// The handle for the active console output device.
	///
	EFI_HANDLE                        ConsoleOutHandle; // 0x38
	///
	/// A pointer to the EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL interface
	/// that is associated with ConsoleOutHandle.
	///
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL   *ConOut; // 0x40
	///
	/// The handle for the active standard error console device.
	/// This handle must support the EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL.
	///
	EFI_HANDLE                        StandardErrorHandle; // 0x48
	///
	/// A pointer to the EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL interface
	/// that is associated with StandardErrorHandle.
	///
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL   *StdErr; // 0x50
	///
	/// A pointer to the EFI Runtime Services Table.
	///
	EFI_RUNTIME_SERVICES              *RuntimeServices; // 0x58
	///
	/// A pointer to the EFI Boot Services Table.
	///
	EFI_BOOT_SERVICES                 *BootServices; // 0x60
	///
	/// The number of system configuration tables in the buffer ConfigurationTable.
	///
	UINTN                             NumberOfTableEntries; // 0x68
	///
	/// A pointer to the system configuration tables.
	/// The number of entries in the table is NumberOfTableEntries.
	///
	EFI_CONFIGURATION_TABLE           *ConfigurationTable; // 0x70
} EFI_SYSTEM_TABLE;
