#pragma once

#include <vector>
#include <string>
#include <cstdint>

void decodeMSGNat(std::vector<uint8_t> data, std::string pro_output_file);

void decodeAVHRRNat(std::vector<uint8_t> data, std::string pro_output_file);
