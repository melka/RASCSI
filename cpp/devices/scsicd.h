//---------------------------------------------------------------------------
//
//	SCSI Target Emulator PiSCSI
//	for Raspberry Pi
//
//	Copyright (C) 2001-2006 ＰＩ．(ytanaka@ipc-tokai.or.jp)
//	Copyright (C) 2014-2020 GIMONS
//	Copyright (C) akuker
//
//	Licensed under the BSD 3-Clause License.
//	See LICENSE file in the project root folder.
//
//---------------------------------------------------------------------------

#pragma once

#include "cd_track.h"
#include "disk.h"
#include "interfaces/scsi_mmc_commands.h"

class SCSICD : public Disk, private ScsiMmcCommands
{
public:

	SCSICD(int, const unordered_set<uint32_t>&, scsi_defs::scsi_level = scsi_level::SCSI_2);
	~SCSICD() override = default;

	bool Init(const unordered_map<string, string>&) override;

	void Open() override;

	vector<uint8_t> InquiryInternal() const override;
	int Read(const vector<int>&, vector<uint8_t>&, uint64_t) override;

protected:

	void SetUpModePages(map<int, vector<byte>>&, int, bool) const override;
	void AddVendorPage(map<int, vector<byte>>&, int, bool) const override;

private:

	int ReadTocInternal(const vector<int>&, vector<uint8_t>&);

	void AddCDROMPage(map<int, vector<byte>>&, bool) const;
	void AddCDDAPage(map<int, vector<byte>>&, bool) const;
	scsi_defs::scsi_level scsi_level;

	void OpenIso();
	void OpenPhysical();

	void CreateDataTrack();

	void ReadToc() override;

	void LBAtoMSF(uint32_t, uint8_t *) const;			// LBA→MSF conversion

	bool rawfile = false;					// RAW flag

	// Track management
	void ClearTrack();						// Clear the track
	int SearchTrack(uint32_t lba) const;	// Track search
	vector<unique_ptr<CDTrack>> tracks;		// Track opbject references
	int dataindex = -1;						// Current data track
	int audioindex = -1;					// Current audio track
};
