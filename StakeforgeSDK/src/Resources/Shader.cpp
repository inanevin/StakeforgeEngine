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

#include "SFG/Resources/Shader.hpp"
#include "SFG/Data/Vector.hpp"
#include "SFG/Data/String.hpp"
#include "SFG/Type/SizeDefinitions.hpp"

#include <slang/slang-com-ptr.h>
#include <slang/slang.h>

namespace SFG
{
	slang::IGlobalSession* Shader::s_slangGlobalSession = nullptr;

	bool Shader::InitializeSlang()
	{
		Slang::ComPtr<slang::IGlobalSession> slangGlobalSession;
		Slang::Result						 result = slang::createGlobalSession(slangGlobalSession.writeRef());
		if (result != SLANG_OK)
			return false;

		s_slangGlobalSession = slangGlobalSession.detach();
		return true;
	}

	void Shader::ShutdownSlang()
	{
		if (s_slangGlobalSession != nullptr)
		{
			s_slangGlobalSession->release();
			s_slangGlobalSession = nullptr;
		}
	}

	void Shader::LoadFromFile(const char* file, const ShaderLoadOptions& loadOptions)
	{
		// First we need to create slang global session with work with the Slang API.
		Slang::ComPtr<slang::IGlobalSession> slangGlobalSession;

		Slang::Result result = slang::createGlobalSession(slangGlobalSession.writeRef());

		// Next we create a compilation session to generate SPIRV code from Slang source.
		slang::SessionDesc sessionDesc = {};
		slang::TargetDesc  targetDesc  = {};
		targetDesc.format			   = SLANG_METAL_LIB;
		// targetDesc.profile = slangGlobalSession->findProfile("spirv_1_5");
		targetDesc.flags = 0;

		sessionDesc.targets		= &targetDesc;
		sessionDesc.targetCount = 1;

		Vector<slang::CompilerOptionEntry> options;
		// options.push_back({slang::CompilerOptionName::EmitSpirvDirectly,{slang::CompilerOptionValueKind::Int, 1, 0, nullptr, nullptr}});
		sessionDesc.compilerOptionEntries	 = options.data();
		sessionDesc.compilerOptionEntryCount = static_cast<uint32>(options.size());

		Slang::ComPtr<slang::ISession> session;
		result = slangGlobalSession->createSession(sessionDesc, session.writeRef());
		/*
				// Once the session has been obtained, we can start loading code into it.
				//
				// The simplest way to load code is by calling `loadModule` with the name of a Slang
				// module. A call to `loadModule("hello-world")` will behave more or less as if you
				// wrote:
				//
				//      import hello_world;
				//
				// In a Slang shader file. The compiler will use its search paths to try to locate
				// `hello-world.slang`, then compile and load that file. If a matching module had
				// already been loaded previously, that would be used directly.
				slang::IModule* slangModule = nullptr;
				{
					Slang::ComPtr<slang::IBlob> diagnosticBlob;

					String path = resourceBase.resolveResource("hello-world.slang");
					slangModule = session->loadModule(path.getBuffer(), diagnosticBlob.writeRef());
					diagnoseIfNeeded(diagnosticBlob);
					if (!slangModule)
						return -1;
				}

				// Loading the `hello-world` module will compile and check all the shader code in it,
				// including the shader entry points we want to use. Now that the module is loaded
				// we can look up those entry points by name.
				//
				// Note: If you are using this `loadModule` approach to load your shader code it is
				// important to tag your entry point functions with the `[shader("...")]` attribute
				// (e.g., `[shader("compute")] void computeMain(...)`). Without that information there
				// is no umambiguous way for the compiler to know which functions represent entry
				// points when it parses your code via `loadModule()`.
				//
				Slang::ComPtr<slang::IEntryPoint> entryPoint;
				slangModule->findEntryPointByName("computeMain", entryPoint.writeRef());

				// At this point we have a few different Slang API objects that represent
				// pieces of our code: `module`, `vertexEntryPoint`, and `fragmentEntryPoint`.
				//
				// A single Slang module could contain many different entry points (e.g.,
				// four vertex entry points, three fragment entry points, and two compute
				// shaders), and before we try to generate output code for our target API
				// we need to identify which entry points we plan to use together.
				//
				// Modules and entry points are both examples of *component types* in the
				// Slang API. The API also provides a way to build a *composite* out of
				// other pieces, and that is what we are going to do with our module
				// and entry points.
				//
				Slang::List<slang::IComponentType*> componentTypes;
				componentTypes.add(slangModule);
				componentTypes.add(entryPoint);

				// Actually creating the composite component type is a single operation
				// on the Slang session, but the operation could potentially fail if
				// something about the composite was invalid (e.g., you are trying to
				// combine multiple copies of the same module), so we need to deal
				// with the possibility of diagnostic output.
				//
				Slang::ComPtr<slang::IComponentType> composedProgram;
				{
					Slang::ComPtr<slang::IBlob> diagnosticsBlob;
					SlangResult result = session->createCompositeComponentType(
																			   componentTypes.getBuffer(),
																			   componentTypes.getCount(),
																			   composedProgram.writeRef(),
																			   diagnosticsBlob.writeRef());
					diagnoseIfNeeded(diagnosticsBlob);
					RETURN_ON_FAIL(result);
				}

				// Now we can call `composedProgram->getEntryPointCode()` to retrieve the
				// compiled SPIRV code that we will use to create a vulkan compute pipeline.
				// This will trigger the final Slang compilation and spirv code generation.
				Slang::ComPtr<slang::IBlob> spirvCode;
				{
					ComPtr<slang::IBlob> diagnosticsBlob;
					SlangResult result = composedProgram->getEntryPointCode(
																			0,
																			0,
																			spirvCode.writeRef(),
																			diagnosticsBlob.writeRef());
					diagnoseIfNeeded(diagnosticsBlob);
					RETURN_ON_FAIL(result);

					if (isTestMode())
					{
						printEntrypointHashes(1, 1, composedProgram);
					}
				}
		 */
	}
} // namespace SFG
