﻿#include "stdafx.h"
#include "stdio.h"
#include <windows.h>
#include "..\includes\injector\injector.hpp"
#include <cstdint>
#include "..\includes\IniReader.h"
#include <d3d9.h>

DWORD WINAPI Thing(LPVOID);

bool HDReflections, BrokenReflectionFix, RestoreSkybox, TrafficSignFix, HiddenVisualTreatment;
static int ResolutionX, ResolutionY, ImproveReflectionLOD, ConsoleReflections;
static float RoadScale, VehicleScale, MirrorScale, ReflectionBlurStrength;
static float DOFStrength = 2.0f;
static float FEReflectionStrength = 4.0f;
static float TrafficSignDistance = 45000.0f;
static float VehicleReflSkyboxRenderDistance = 0.1f;
__int64 VehicleReflBrightness = 0x3F0000003FCCCCCD;
DWORD GameState;
HWND windowHandle;

DWORD RoadReflectionCodeCaveExit1 = 0x71AA2B;
DWORD RoadReflectionRes1 = 0x00000000;
DWORD RoadReflectionCodeCaveExit2 = 0x71AA7B;
DWORD RoadReflectionRes2 = 0x00000000;
DWORD DownScale4x4ReturnAddress;
DWORD DownScale4x4ReturnAddressCodeCaveExit = 0x748886;
DWORD DownScale4x4StrengthCodeCaveExit1 = 0x73CC2F;
DWORD DownScale4x4StrengthCodeCaveExit2 = 0x73CC43;
DWORD TrafficSignFixCodeCaveExit = 0x71E085;
DWORD ImproveReflectionLODCodeCaveExit = 0x79AE9A;
DWORD VehicleReflShaderCodeCaveExit = 0x729461;
DWORD BrokenVehicleReflFixCodeCave1Exit = 0x71B8A6;
DWORD BrokenVehicleReflFixCodeCave2Exit = 0x721E0E;
DWORD BrokenVehicleReflFixCodeCave3Exit = 0x73E1BA;
DWORD VehicleReflSkyboxCodeCaveExit = 0x7AFCAA;
DWORD VehicleReflSkyboxCodeCavePart2Exit = 0x7AFCD5;
DWORD VehicleReflSkyboxCodeCavePart2Jump = 0x7AFCAA;
DWORD VehicleReflBrightnessCodeCaveExit = 0x748A81;
DWORD sub_713B30 = 0x713B30;

void __declspec(naked) RoadReflectionCodeCave1()
{
	__asm {
		fild dword ptr ds : [ResolutionX]
		fmul dword ptr ds : [RoadScale]
		fistp dword ptr ds : [RoadReflectionRes1]
		mov edx, dword ptr ds : [RoadReflectionRes1]
		push edx
		push eax
		call dword ptr ds : [ecx + 0x5C]
		jmp RoadReflectionCodeCaveExit1
	}
}

void __declspec(naked) RoadReflectionCodeCave2()
{
	__asm {
		fild dword ptr ds : [ResolutionX]
		fmul dword ptr ds : [RoadScale]
		fistp dword ptr ds : [RoadReflectionRes2]
		mov ecx, dword ptr ds : [RoadReflectionRes2]
		push ecx
		push eax
		call dword ptr ds : [edx + 0x74]
		jmp RoadReflectionCodeCaveExit2
	}
}

void __declspec(naked) DownScale4x4ReturnAddressCodeCave()
{
	__asm {
		// gets return address
		push eax
		mov eax, dword ptr ds : [esp + 0x4]
		mov dword ptr ds : [DownScale4x4ReturnAddress], eax
		pop eax
		push ebp
		mov ebp, esp
		and esp, 0xFFFFFFF0
		jmp DownScale4x4ReturnAddressCodeCaveExit
	}
}

void __declspec(naked) DownScale4x4StrengthCodeCave1()
{
	__asm {
		cmp dword ptr ds : [DownScale4x4ReturnAddress], 0x72E22E
		je DownScale4x4StrengthCodeCave1Part2
		cmp dword ptr ds : [DownScale4x4ReturnAddress], 0x726B17
		je DownScale4x4StrengthCodeCave1Part3
		
		// restores horizontal depth of field
		fdivr dword ptr ds : [DOFStrength]
		jmp DownScale4x4StrengthCodeCaveExit1
		
	DownScale4x4StrengthCodeCave1Part2 :
		// horizontal road reflection
		fdivr dword ptr ds : [ReflectionBlurStrength]
		jmp DownScale4x4StrengthCodeCaveExit1

	DownScale4x4StrengthCodeCave1Part3 :
		// restores horizontal frontend blur
		fdivr dword ptr ds : [FEReflectionStrength]
			jmp DownScale4x4StrengthCodeCaveExit1	
	}
}

void __declspec(naked) DownScale4x4StrengthCodeCave2()
{
	__asm {
		cmp dword ptr ds : [DownScale4x4ReturnAddress], 0x72E22E
		je DownScale4x4StrengthCodeCave2Part2
		cmp dword ptr ds : [DownScale4x4ReturnAddress], 0x726B17
		je DownScale4x4StrengthCodeCave2Part3
		
		// restores vertical depth of field
		fdivr dword ptr ds : [DOFStrength]
		jmp DownScale4x4StrengthCodeCaveExit2

	DownScale4x4StrengthCodeCave2Part2 :
		// vertical road reflection
		fdivr dword ptr ds : [ReflectionBlurStrength]
		jmp DownScale4x4StrengthCodeCaveExit2

	DownScale4x4StrengthCodeCave2Part3 :
		// restores vertical frontend blur
		fdivr dword ptr ds : [FEReflectionStrength]
		jmp DownScale4x4StrengthCodeCaveExit2
	}
}

void __declspec(naked) TrafficSignFixCodeCave()
{
	__asm {
		// traffic sign brightness
		mov ecx, dword ptr ds : [0xA6B910]
		mov dword ptr ds : [ecx + 0x74], 0x41000000
		fld dword ptr ds : [esi + 0x6C]
		fmul dword ptr ds : [esi + 0x64]
		jmp TrafficSignFixCodeCaveExit
	}
}

void __declspec(naked) ImproveReflectionLODCodeCave()
{
	__asm {
		cmp dword ptr ds : [ebx + 0x08], 0x03
		je ImproveReflectionLODCodeCave2
		mov edi, dword ptr ds : [ebx + 0x08]
		lea ecx, dword ptr ds : [edx + edx * 0x4]
		jmp ImproveReflectionLODCodeCaveExit

	ImproveReflectionLODCodeCave2 :
		// for mirror LOD
		mov edi, 0x01
		lea ecx, dword ptr ds : [edx + edx * 4]
		jmp ImproveReflectionLODCodeCaveExit
	}
}

void __declspec(naked) VehicleReflShaderCodeCave()
{
	__asm {
		mov ecx, dword ptr ds : [edi + 0x04]
		cmp ecx, 0x01
		je VehicleReflShaderCodeCavePart2
		jmp VehicleReflShaderCodeCaveExit

	VehicleReflShaderCodeCavePart2 :
		push edi
		mov edi, [esp + 0x0C]
		mov edi, [edi + 0x08]
		cmp edi, 0x16
		jnl VehicleReflShaderCodeCavePart3
		pop edi
		jmp VehicleReflShaderCodeCaveExit

	VehicleReflShaderCodeCavePart3 :
		pop edi
		movzx ecx, ax
		mov edi, dword ptr ds : [ecx * 0x4 + 0xB1F66C]
		jmp VehicleReflShaderCodeCaveExit
	}
}

void __declspec(naked) BrokenVehicleReflFixCodeCave1()
{
	__asm {
		cmp eax, 0x16
		jl BrokenVehicleReflFixCodeCave1Conditional2
		mov dword ptr ds : [esp + 0x24], 0x3F000000
		cmp eax, 0x1A
		je BrokenVehicleReflFixCodeCave1Conditional1
		cmp eax, 0x1B
		jne BrokenVehicleReflFixCodeCave1Conditional2

	BrokenVehicleReflFixCodeCave1Conditional1:
		// Top center and Bottom center
		mov dword ptr ds : [esp + 0x24], 0x3F800000

	BrokenVehicleReflFixCodeCave1Conditional2:
		movzx ecx, word ptr ds : [edi + 0xE4]
		jmp BrokenVehicleReflFixCodeCave1Exit
	}
}

void __declspec(naked) BrokenVehicleReflFixCodeCave2()
{
	__asm {
		mov dword ptr ds : [ebx + 0xE4], 0x4000
		push eax
		lea eax, dword ptr ds : [ebx + 0xE4]
		cmp eax, 0xAB1664
		je BrokenVehicleReflFixCodeCave2Conditional
		cmp eax, 0xAB1954
		je BrokenVehicleReflFixCodeCave2Conditional
		jmp BrokenVehicleReflFixCodeCave2Part2

	BrokenVehicleReflFixCodeCave2Conditional :
		mov dword ptr ds : [ebx + 0xE4], 0x6000
	
	BrokenVehicleReflFixCodeCave2Part2 :
		pop eax
		jmp BrokenVehicleReflFixCodeCave2Exit
	}
}

void __declspec(naked) BrokenVehicleReflFixCodeCave3()
{
	__asm {
		cmp eax, 0x3ECCCCCD
		jne BrokenVehicleReflFixCodeCave3Conditional
		mov eax, 0x00
	
	BrokenVehicleReflFixCodeCave3Conditional :
		push eax
		mov ecx, esi
		push 0x37
		jmp BrokenVehicleReflFixCodeCave3Exit
	}
}

void __declspec(naked) VehicleReflSkyboxCodeCave()
{
	__asm {
		cmp eax, 0x03
		jg VehicleReflSkyboxCodeCavePart2
		jmp VehicleReflSkyboxCodeCaveExit

	VehicleReflSkyboxCodeCavePart2 :
		mov eax, dword ptr ds : [0xA7335C]
		test eax, eax
		je VehicleReflSkyboxCodeCavePart2Conditional
		test ecx, ecx
		fld dword ptr ds : [VehicleReflSkyboxRenderDistance]
		fmul st(0), st(1)
		fmul dword ptr ds: [esi]
		fstp dword ptr ds : [esi]
		fld dword ptr ds : [VehicleReflSkyboxRenderDistance]
		fmul st(0), st(1)
		fmul dword ptr ds : [esi + 0x14]
		fstp dword ptr ds : [esi + 0x14]
		fld dword ptr ds : [VehicleReflSkyboxRenderDistance]
		jmp VehicleReflSkyboxCodeCavePart2Exit

	VehicleReflSkyboxCodeCavePart2Conditional :
		jmp VehicleReflSkyboxCodeCavePart2Jump
	}
}

void __declspec(naked) VehicleReflBrightnessCodeCave()
{
	__asm {
		mov eax, dword ptr ds : [0xAB08F4]
		mov eax, dword ptr ds : [eax + 0x04]
		cmp eax, 0x16
		jnl VehicleReflBrightnessCodeCavePart2
		cmp dword ptr ds : [0xA99BBC], 0x06
		jmp VehicleReflBrightnessCodeCaveExit

	VehicleReflBrightnessCodeCavePart2 :
		cmp dword ptr ds : [0xA99BBC], 06
		mov dword ptr ds : [0xA6C210], 0x3ECCCCCD
		lea eax, dword ptr ds : [VehicleReflBrightness]
		je VehicleReflBrightnessCodeCaveConditional
		mov eax, 0xA6C214

	VehicleReflBrightnessCodeCaveConditional :
		push eax
		push 0x8B
		call sub_713B30
		ret
	}
}

void Init()
{
	// Read values from .ini
	CIniReader iniReader("NFSCHDReflections.ini");

	// Resolution
	ResolutionX = iniReader.ReadInteger("RESOLUTION", "ResolutionX", 1920);
	ResolutionY = iniReader.ReadInteger("RESOLUTION", "ResolutionY", 1080);
	RoadScale = iniReader.ReadFloat("RESOLUTION", "RoadScale", 1.0);
	VehicleScale = iniReader.ReadFloat("RESOLUTION", "VehicleScale", 1.0);
	MirrorScale = iniReader.ReadFloat("RESOLUTION", "MirrorScale", 1.0);

	// General
	HDReflections = iniReader.ReadInteger("GENERAL", "HDReflections", 1);
	ImproveReflectionLOD = iniReader.ReadInteger("GENERAL", "ImproveReflectionLOD", 1);
	ConsoleReflections = iniReader.ReadInteger("GENERAL", "ConsoleReflections", 2);
	BrokenReflectionFix = iniReader.ReadInteger("GENERAL", "BrokenReflectionFix", 1);
	RestoreSkybox = iniReader.ReadInteger("GENERAL", "RestoreSkybox", 1);
	ReflectionBlurStrength = iniReader.ReadFloat("GENERAL", "ReflectionBlurStrength", 2.0f);
	
	// Extra
	TrafficSignFix = iniReader.ReadInteger("EXTRA", "TrafficSignFix", 1);
	HiddenVisualTreatment = iniReader.ReadInteger("EXTRA", "HiddenVisualTreatment", 0);

	if (HDReflections)
	{
		// Road Reflection X
		injector::MakeJMP(0x71AA26, RoadReflectionCodeCave1, true);
		injector::MakeJMP(0x71AA76, RoadReflectionCodeCave2, true);
		injector::WriteMemory<uint32_t>(0x71BE28, ResolutionX * RoadScale, true);
		// Road Reflection Y
		injector::MakeNOP(0x71A9FC, 2, true);
		injector::WriteMemory<uint32_t>(0x71A9F8, ResolutionY * RoadScale, true);
		injector::WriteMemory<uint32_t>(0x71BE2F, ResolutionY * RoadScale, true);
		// Vehicle Reflection
		injector::WriteMemory<uint32_t>(0x70DE39, ResolutionY * VehicleScale, true);
		// RVM Reflection
		// Aspect ratio is based on NFSU2 because true aspect ratio is unknown
		injector::WriteMemory<uint32_t>(0x70DB04, ResolutionY * MirrorScale, true);
		injector::MakeNOP(0x70DB08, 2, true);
		injector::WriteMemory<uint32_t>(0x70DB62, (ResolutionY / 3) * MirrorScale, true);
		injector::MakeNOP(0x70DB66, 2, true);
	}

	if (ImproveReflectionLOD >= 1)
	{
		// Vehicle Reflection LOD
		injector::MakeNOP(0x79FACD, 2, true);
		// RVM Reflection LOD
		injector::WriteMemory<uint32_t>(0x710427, 0x00000000, true);
		
		if (ImproveReflectionLOD >= 2)
		// Full LOD Improvement
		injector::MakeJMP(0x79AE94, ImproveReflectionLODCodeCave, true);

		// I don't know why I have to use the same conditional statement for this to work
		if (ImproveReflectionLOD >= 2)
		injector::WriteMemory<uint8_t>(0x79FB5C, 0x10, true);
	}

	if (ConsoleReflections >= 1)
	{
		// Changes the vehicle reflection shader to match the PS3 version
		injector::MakeJMP(0x729445, VehicleReflShaderCodeCave, true);

		if (ConsoleReflections >= 2)
		// Darkens the road, buildings, and other objects to match the Xbox 360 version
		injector::MakeJMP(0x748A7A, VehicleReflBrightnessCodeCave, true);
	}

	if (BrokenReflectionFix)
	{
		// Corrects the FOV so all segments of the vehicle reflection aligns correctly
		injector::MakeJMP(0x71B89F, BrokenVehicleReflFixCodeCave1, true);
		// Corrects the FOV so all segments of the vehicle reflection aligns correctly
		injector::MakeJMP(0x721E07, BrokenVehicleReflFixCodeCave2, true);
		// Restores the original aspect ratio
		injector::MakeJMP(0x73E1B5, BrokenVehicleReflFixCodeCave3, true);
		// Restores missing chunks
		injector::WriteMemory<uint32_t>(0xA63B10, 0x3EA66666, true);
		// Solves an issue that caused the vehicle reflections to use the same geometric detail as the road reflections
		injector::WriteMemory<uint32_t>(0x79AEDD, 0x00008002, true);
	}

	if (RestoreSkybox)
	{
		// Increases the skybox render distance for vehicle reflections
		injector::MakeJMP(0x7AFCA1, VehicleReflSkyboxCodeCave, true);
		// Corrects the position of the skybox for road reflections
		injector::WriteMemory<uint32_t>(0x9EF050, 0x3F400000, true);
	}

	if (TrafficSignFix)
	{
		// Restores the traffic sign brightness
		injector::MakeJMP(0x71E07F, TrafficSignFixCodeCave, true);
		// TrafficSignDistance
		injector::WriteMemory(0x79B257, &TrafficSignDistance, true);
	}

	if (HiddenVisualTreatment)
	{
		// VT pointer
		injector::WriteMemory<uint32_t>(0x71D702, 0xB4306C, true);
	}
	
	if (ReflectionBlurStrength >= 0)
	{
		// Jumps
		injector::MakeJMP(0x748880, DownScale4x4ReturnAddressCodeCave, true);
		injector::MakeJMP(0x73CC29, DownScale4x4StrengthCodeCave1, true);
		injector::MakeJMP(0x73CC3D, DownScale4x4StrengthCodeCave2, true);
		// Allows blur to render at full resolution
		injector::MakeNOP(0x713108, 2, true);
		injector::MakeNOP(0x713141, 2, true);
		injector::MakeNOP(0x73C359, 2, true);
		injector::MakeNOP(0x73C371, 2, true);
		// TwoPassBlur
		injector::WriteMemory(0x73C97E, &ReflectionBlurStrength, true);
		// GaussBlur5x5
		injector::WriteMemory(0x73C68E, &DOFStrength, true);
		injector::WriteMemory(0x73C6A8, &DOFStrength, true);
	}
}
	

BOOL APIENTRY DllMain(HMODULE /*hModule*/, DWORD reason, LPVOID /*lpReserved*/)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		uintptr_t base = (uintptr_t)GetModuleHandleA(NULL);
		IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)(base);
		IMAGE_NT_HEADERS* nt = (IMAGE_NT_HEADERS*)(base + dos->e_lfanew);

		if ((base + nt->OptionalHeader.AddressOfEntryPoint + (0x400000 - base)) == 0x87E926) // Check if .exe file is compatible - Thanks to thelink2012 and MWisBest
			Init();

		else
		{
			MessageBoxA(NULL, "This .exe is not supported.\nPlease use v1.4 English nfsc.exe (6,88 MB (7.217.152 bytes)).", "NFSC HD Reflections", MB_ICONERROR);
			return FALSE;
		}
	}
	return TRUE;
	
}
