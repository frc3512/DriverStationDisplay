////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2012 Laurent Gomila (laurent.gom@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

/* !!! THIS IS AN EXTREMELY ALTERED AND PURPOSE-BUILT VERSION OF SFML !!!
 * This distribution is designed to possess only a limited subset of the
 * original library's functionality and to only build on Win32 platforms.
 * The original distribution of this software has many more features and
 * supports more platforms.
 */

#ifndef SFML_IMAGELOADER_HPP
#define SFML_IMAGELOADER_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "../SFML/System/NonCopyable.hpp"
#include "../SFML/System/Vector2.hpp"
#include <string>
#include <vector>


namespace sf
{

namespace priv
{
////////////////////////////////////////////////////////////
/// \brief Load/save image files
///
////////////////////////////////////////////////////////////
class ImageLoader : NonCopyable
{
public :

    ////////////////////////////////////////////////////////////
    /// \brief Get the unique instance of the class
    ///
    /// \return Reference to the ImageLoader instance
    ///
    ////////////////////////////////////////////////////////////
    static ImageLoader& getInstance();

    ////////////////////////////////////////////////////////////
    /// \brief Load an image from a file in memory
    ///
    /// \param data     Pointer to the file data in memory
    /// \param dataSize Size of the data to load, in bytes
    /// \param pixels   Array of pixels to fill with loaded image
    /// \param size     Size of loaded image, in pixels
    ///
    /// \return True if loading was successful
    ///
    ////////////////////////////////////////////////////////////
    bool loadImageFromMemory(const void* data, std::size_t dataSize, std::vector<uint8_t>& pixels, Vector2<unsigned int>& size);

    ////////////////////////////////////////////////////////////
    /// \bref Save an array of pixels as an image file
    ///
    /// \param filename Path of image file to save
    /// \param pixels   Array of pixels to save to image
    /// \param size     Size of image to save, in pixels
    ///
    /// \return True if saving was successful
    ///
    ////////////////////////////////////////////////////////////
    bool saveImageToFile(const std::string& filename, const std::vector<uint8_t>& pixels, const Vector2<unsigned int>& size);

private :

    ////////////////////////////////////////////////////////////
    /// \brief Default constructor
    ///
    ////////////////////////////////////////////////////////////
    ImageLoader();

    ////////////////////////////////////////////////////////////
    /// \brief Destructor
    ///
    ////////////////////////////////////////////////////////////
    ~ImageLoader();
};

} // namespace priv

} // namespace sf


#endif // SFML_IMAGELOADER_HPP
