cmake_minimum_required (VERSION 3.16)

set(SPECKLE_ADDON_LIB SpeckleAddOn)
set(SPECKLE_ADDON_RESOURCES SpeckleAddOnResources)

function (SetCompilerOptions target)
	target_compile_features (${target} PUBLIC cxx_std_14)
	target_compile_options (${target} PUBLIC "$<$<CONFIG:Debug>:-DDEBUG>")
	if (WIN32)
		target_compile_options (${target} PUBLIC /W4
			/Zc:wchar_t-
			/wd4499
		)
	else ()
		target_compile_options (${target} PUBLIC -Wall -Wextra
			-fvisibility=hidden
			-Wno-multichar
			-Wno-ctor-dtor-privacy
			-Wno-invalid-offsetof
			-Wno-ignored-qualifiers
			-Wno-reorder
			-Wno-overloaded-virtual
			-Wno-unused-parameter
			-Wno-deprecated)
	endif ()
endfunction ()

set_property (GLOBAL PROPERTY USE_FOLDERS ON)

set (CMAKE_SUPPRESS_REGENERATION 1)
set (CMAKE_CONFIGURATION_TYPES Debug;Release;RelWithDebInfo)
set (SPECKLE_ADDON_NAME "Speckle")

if (WIN32)
	add_definitions (-DUNICODE -D_UNICODE)
else ()
	add_definitions (-Dmacintosh=1)
endif ()
add_definitions (-DACExtension)

project (${SPECKLE_ADDON_NAME})

set (AddOnSourcesFolder Sources/AddOn)
set (AddOnResourcesFolder Sources/AddOnResources)

# AddOnResources
set (ResourceObjectsDir ${CMAKE_BINARY_DIR}/AddOns/${SPECKLE_ADDON_NAME}/ResourceObjects)

configure_file (${AddOnResourcesFolder}/ResourceMDIDIds.hpp.in ${ResourceObjectsDir}/ResourceMDIDIds.hpp)

file (GLOB AddOnImageFiles
	${AddOnResourcesFolder}/RFIX/Images/*.svg
)
if (WIN32)
	file (GLOB AddOnResourceFiles
		${AddOnResourcesFolder}/R${AC_ADDON_LANGUAGE}/*.grc
		${AddOnResourcesFolder}/RFIX/*.grc
		${AddOnResourcesFolder}/RFIX/*.grc
		${AddOnResourcesFolder}/RFIX.win/*.rc2
		${AddOnResourcesFolder}/Tools/*.py
	)
else ()
	file (GLOB AddOnResourceFiles
		${AddOnResourcesFolder}/R${AC_ADDON_LANGUAGE}/*.grc
		${AddOnResourcesFolder}/RFIX/*.grc
		${AddOnResourcesFolder}/RFIX.mac/*.plist
		${AddOnResourcesFolder}/Tools/*.py
	)
endif ()

source_group ("Images" FILES ${AddOnImageFiles})
source_group ("Resources" FILES ${AddOnResourceFiles})
add_custom_target (
	${SPECKLE_ADDON_RESOURCES} ALL
	DEPENDS "${ResourceObjectsDir}/AddOnResources.stamp"
		Sources ${AddOnResourceFiles} ${AddOnImageFiles}
)

get_filename_component (AddOnSourcesFolderAbsolute "${CMAKE_CURRENT_LIST_DIR}/${AddOnSourcesFolder}" ABSOLUTE)
get_filename_component (AddOnResourcesFolderAbsolute "${CMAKE_CURRENT_LIST_DIR}/${AddOnResourcesFolder}" ABSOLUTE)
if (WIN32)
	add_custom_command (
		OUTPUT "${ResourceObjectsDir}/AddOnResources.stamp"
		DEPENDS ${AddOnResourceFiles} ${AddOnImageFiles}
		COMMENT "Compiling resources..."
		COMMAND ${CMAKE_COMMAND} -E make_directory "${ResourceObjectsDir}"
		COMMAND python "${AddOnResourcesFolderAbsolute}/Tools/CompileResources.py" "${AC_ADDON_LANGUAGE}" "${AC_API_DEVKIT_DIR}" "${AddOnSourcesFolderAbsolute}" "${AddOnResourcesFolderAbsolute}" "${ResourceObjectsDir}" "${ResourceObjectsDir}/${SPECKLE_ADDON_NAME}.res"
		COMMAND ${CMAKE_COMMAND} -E touch "${ResourceObjectsDir}/AddOnResources.stamp"
	)
else ()
	add_custom_command (
		OUTPUT "${ResourceObjectsDir}/AddOnResources.stamp"
		DEPENDS ${AddOnResourceFiles} ${AddOnImageFiles}
		COMMENT "Compiling resources..."
		COMMAND ${CMAKE_COMMAND} -E make_directory "${ResourceObjectsDir}"
		COMMAND python "${AddOnResourcesFolderAbsolute}/Tools/CompileResources.py" "${AC_ADDON_LANGUAGE}" "${AC_API_DEVKIT_DIR}" "${AddOnSourcesFolderAbsolute}" "${AddOnResourcesFolderAbsolute}" "${ResourceObjectsDir}" "${CMAKE_BINARY_DIR}/$<CONFIG>/${SPECKLE_ADDON_NAME}.bundle/Contents/Resources"
		COMMAND ${CMAKE_COMMAND} -E copy "${AC_API_DEVKIT_DIR}/Support/Inc/PkgInfo" "${CMAKE_BINARY_DIR}/$<CONFIG>/${SPECKLE_ADDON_NAME}.bundle/Contents/PkgInfo"
		COMMAND ${CMAKE_COMMAND} -E touch "${ResourceObjectsDir}/AddOnResources.stamp"
	)
endif ()

# AddOn
file (GLOB AddOnHeaderFiles
	${AddOnSourcesFolder}/*.h
	${AddOnSourcesFolder}/*.hpp
)
file (GLOB AddOnSourceFiles
	${AddOnSourcesFolder}/*.c
	${AddOnSourcesFolder}/*.cpp
	${AddOnSourcesFolder}/*.cc
)
set (
	AddOnFiles
	${AddOnHeaderFiles}
	${AddOnSourceFiles})
source_group ("Sources" FILES ${AddOnFiles})
if (WIN32)
	add_library (${SPECKLE_ADDON_LIB} SHARED ${AddOnFiles})
else ()
	add_library (${SPECKLE_ADDON_LIB} MODULE ${AddOnFiles})
endif ()

set_target_properties (${SPECKLE_ADDON_LIB} PROPERTIES OUTPUT_NAME ${SPECKLE_ADDON_NAME})
if (WIN32)
	set_target_properties (${SPECKLE_ADDON_LIB} PROPERTIES SUFFIX ".apx")
else ()
	set_target_properties (${SPECKLE_ADDON_LIB} PROPERTIES BUNDLE TRUE)
	set_target_properties (${SPECKLE_ADDON_LIB} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_LIST_DIR}/${AddOnResourcesFolder}/RFIX.mac/Info.plist")
	set_target_properties (${SPECKLE_ADDON_LIB} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${AC_ADDON_LANGUAGE}/$<CONFIG>")
endif ()

if (WIN32)
	target_link_options (${SPECKLE_ADDON_LIB} PUBLIC "${ResourceObjectsDir}/${SPECKLE_ADDON_NAME}.res")
	target_link_options (${SPECKLE_ADDON_LIB} PUBLIC /export:GetExportedFuncAddrs,@1 /export:SetImportedFuncAddrs,@2)
endif ()

target_include_directories (${SPECKLE_ADDON_LIB} PUBLIC
		${AddOnSourcesFolder}
		${AC_API_DEVKIT_DIR}/Support/Inc
		${LIB_INCLUDE_DIRS}
)

if (WIN32)
	target_link_libraries (${SPECKLE_ADDON_LIB}
		"$<$<CONFIG:Debug>:${AC_API_DEVKIT_DIR}/Support/Lib/Win/ACAP_STATD.lib>"
		"$<$<CONFIG:Release>:${AC_API_DEVKIT_DIR}/Support/Lib/Win/ACAP_STAT.lib>"
		"$<$<CONFIG:RelWithDebInfo>:${AC_API_DEVKIT_DIR}/Support/Lib/Win/ACAP_STAT.lib>"
	)
else ()
	find_library (CocoaFramework Cocoa)
	target_link_libraries (${SPECKLE_ADDON_LIB}
		"${AC_API_DEVKIT_DIR}/Support/Lib/Mactel/libACAP_STAT.a"
		${CocoaFramework}
	)
endif ()

SetCompilerOptions (${SPECKLE_ADDON_LIB})
set_source_files_properties (${AddOnSourceFiles} PROPERTIES LANGUAGE CXX)

file (GLOB ModuleFolders ${AC_API_DEVKIT_DIR}/Support/Modules/*)
target_include_directories (${SPECKLE_ADDON_LIB} PUBLIC ${ModuleFolders})
if (WIN32)
	file (GLOB LibFilesInFolder ${AC_API_DEVKIT_DIR}/Support/Modules/*/*/*.lib)
	target_link_libraries (${SPECKLE_ADDON_LIB} ${LibFilesInFolder})
else ()
	file (GLOB FrameworkFilesInFolder ${AC_API_DEVKIT_DIR}/Support/Frameworks/*.framework)
	target_link_libraries (${SPECKLE_ADDON_LIB} ${FrameworkFilesInFolder})
endif ()
add_dependencies (${SPECKLE_ADDON_LIB} ${SPECKLE_ADDON_RESOURCES})


# group to folder
set_target_properties(${SPECKLE_ADDON_RESOURCES} PROPERTIES FOLDER Resources)

set_target_properties (${SPECKLE_ADDON_LIB} PROPERTIES VS_DEBUGGER_COMMAND "C:\\Program Files\\GRAPHISOFT_INT\\Archicad 27\\Archicad.exe")

# other libs
target_link_libraries(${SPECKLE_ADDON_LIB} json)

#compile definitions
SetCompilerParams(${SPECKLE_ADDON_LIB})


