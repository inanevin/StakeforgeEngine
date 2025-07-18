// Copyright (c) 2025 Inan Evin

#pragma once

#include "common/size_definitions.hpp"
#include "data/vector.hpp"
#include "data/string.hpp"

namespace Game
{
	class file_system
	{
	public:
		static bool	  delete_file(const char* path);
		static bool	  create_directory(const char* path);
		static bool	  delete_directory(const char* path);
		static void	  get_files_in_directory(const char* path, vector<string>& out_data, string extension_filter = "");
		static void	  get_all_in_directory(const char* path, vector<string>& out_data);
		static bool	  is_directory(const char* path);
		static bool	  change_directory_name(const char* oldPath, const char* new_path);
		static bool	  exists(const char* path);
		static string get_last_modified_date(const char* path);
		static string get_directory_of_file(const char* path);
		static string remove_extensions_from_path(const string& filename);
		static string get_filename_and_extension_from_path(const string& filename);
		static string get_file_extension(const string& file);
		static string get_filename_from_path(const string& file);
		static string get_last_folder_from_path(const string& path);
		static string read_file_as_string(const string& file_path);
		static string get_running_directory();
		static string get_user_directory();
		static string fix_path(const string& str);
		static string duplicate(const char* path);
		static string get_relative(const char* src, const char* target);
		static string get_system_time_str();
		static string get_time_str_from_microseconds(int64 microseconds);
		static void	  read_file_as_vector(const char* file_path, vector<char>& vec);
		static void	  perform_move(const char* target_file, const char* target_dir);
		static void	  get_sys_time_ints(int32& hours, int32& minutes, int32& seconds);
		static void	  copy_directory(const char* copyDir, const char* target_parent_folder);
		static void	  copy_file_to_directory(const char* file, const char* target_parent_folder);
	};

} // namespace SFG
