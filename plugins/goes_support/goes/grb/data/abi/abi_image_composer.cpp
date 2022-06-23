#include "abi_image_composer.h"
#include <ctime>
#include <filesystem>
#include "logger.h"

namespace goes
{
    namespace grb
    {
        ABIComposer::ABIComposer(std::string dir, products::ABI::ABIScanType abi_type)
            : abi_directory(dir),
              abi_product_type(abi_type)
        {
            current_timestamp = 0;
            reset();
        }

        ABIComposer::~ABIComposer()
        {
            if (has_data())
                save();
        }

        void ABIComposer::reset()
        {
            for (int i = 0; i < 16; i++)
            {
                channel_images[i].clear();
                has_channels[i] = false;
            }
        }

        bool ABIComposer::has_data()
        {
            for (int i = 0; i < 16; i++)
                if (has_channels[i])
                    return true;
            return false;
        }

        void ABIComposer::feed_channel(double timestamp, int ch, image::Image<uint16_t> &img)
        {
            if (ch >= 16)
                return;

            if (timestamp != current_timestamp)
            {
                if (has_data())
                    save();
                reset();
                current_timestamp = timestamp;
            }

            channel_images[ch - 1] = img;
            has_channels[ch - 1] = true;

            // logger->critical("Channel compose got {:d} at {:f}", ch, timestamp);
        }

        void ABIComposer::saveABICompo(image::Image<uint16_t> img, std::string name)
        {

            std::string zone = products::ABI::abiZoneToString(abi_product_type);
            time_t time_tt = current_timestamp;
            std::tm *timeReadable = gmtime(&time_tt);
            std::string utc_filename = std::to_string(timeReadable->tm_year + 1900) +                                                                               // Year yyyy
                                       (timeReadable->tm_mon + 1 > 9 ? std::to_string(timeReadable->tm_mon + 1) : "0" + std::to_string(timeReadable->tm_mon + 1)) + // Month MM
                                       (timeReadable->tm_mday > 9 ? std::to_string(timeReadable->tm_mday) : "0" + std::to_string(timeReadable->tm_mday)) + "T" +    // Day dd
                                       (timeReadable->tm_hour > 9 ? std::to_string(timeReadable->tm_hour) : "0" + std::to_string(timeReadable->tm_hour)) +          // Hour HH
                                       (timeReadable->tm_min > 9 ? std::to_string(timeReadable->tm_min) : "0" + std::to_string(timeReadable->tm_min)) +             // Minutes mm
                                       (timeReadable->tm_sec > 9 ? std::to_string(timeReadable->tm_sec) : "0" + std::to_string(timeReadable->tm_sec)) + "Z";

            std::string filename = "ABI_" + zone + "_" + name + "_" + utc_filename + ".png";
            std::string directory = abi_directory + "/" + zone + "/" + utc_filename + "/";
            std::filesystem::create_directories(directory);

            logger->info("Saving " + directory + filename);
            img.save_png(std::string(directory + filename).c_str());
        }

        void ABIComposer::save()
        {
            // Check if we can make 1-3-5
            if (has_channels[0] && has_channels[2] && has_channels[4])
            {
                image::Image<uint16_t> compo(channel_images[0].width(), channel_images[0].height(), 3);
                compo.draw_image(0, channel_images[4]);
                compo.draw_image(1, channel_images[2]);
                compo.draw_image(2, channel_images[0]);
                saveABICompo(compo, "RGB135");
            }
        }
    }
}