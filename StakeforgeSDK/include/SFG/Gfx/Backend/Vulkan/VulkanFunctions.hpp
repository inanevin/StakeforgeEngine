/*
This file is a part of: Stakeforge Engine
https://github.com/inanevin/StakeforgeEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2025-] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

struct VkDevice_T;
struct VkCommandBuffer_T;
struct VkRenderingInfo;
struct VkDebugUtilsObjectNameInfoEXT;
struct VkDebugUtilsLabelEXT;
enum VkResult;

namespace SFG
{
	typedef VkResult (*SetDebugUtilsObjectName)(VkDevice_T* device, const VkDebugUtilsObjectNameInfoEXT* info);
	typedef void	 (*BeginRendering)(VkCommandBuffer_T* cmd, const VkRenderingInfo* info);
	typedef void	 (*EndRendering)(VkCommandBuffer_T* cmd);
	typedef void	 (*BeginDebugUtilsLabel)(VkCommandBuffer_T* cmd, const VkDebugUtilsLabelEXT* info);
	typedef void	 (*EndDebugUtilsLabel)(VkCommandBuffer_T* cmd);

	extern SetDebugUtilsObjectName pfn_setDebugUtilsObjectNameEXT;
	extern BeginRendering		   pfn_cmdBeginRendering;
	extern EndRendering			   pfn_cmdEndRendering;
	extern BeginDebugUtilsLabel	   pfn_cmdBeginDebugUtilsLabel;
	extern EndDebugUtilsLabel	   pfn_cmdEndDebugUtilsLabel;

#ifdef SFG_DEBUG
#define VK_NAME_OBJECT(namedObject, objType, name, structName)                                                                                                                                                                                                     \
	VkDebugUtilsObjectNameInfoEXT structName = {};                                                                                                                                                                                                                 \
	structName.sType						 = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;                                                                                                                                                                 \
	structName.objectType					 = objType;                                                                                                                                                                                                            \
	structName.objectHandle					 = (uint64_t)namedObject;                                                                                                                                                                                              \
	structName.pObjectName					 = name;                                                                                                                                                                                                               \
	pfn_setDebugUtilsObjectNameEXT(m_device, &structName);
#elif
#define VK_NAME_OBJECT(...)
#endif
}; // namespace SFG
