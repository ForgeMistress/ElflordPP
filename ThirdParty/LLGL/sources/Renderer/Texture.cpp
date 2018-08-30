/*
 * Texture.cpp
 * 
 * This file is part of the "LLGL" project (Copyright (c) 2015-2018 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <LLGL/Texture.h>


namespace LLGL
{


Texture::Texture(const TextureType type) :
    type_ { type }
{
}

ResourceType Texture::QueryResourceType() const
{
    return ResourceType::Texture;
}


} // /namespace LLGL



// ================================================================================
