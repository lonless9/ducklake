#ifndef DUCKDB_BUILD_LOADABLE_EXTENSION
#define DUCKDB_BUILD_LOADABLE_EXTENSION
#endif
#include "ducklake_extension.hpp"
#include "duckdb.hpp"
#include "duckdb/common/exception.hpp"
#include "duckdb/common/string_util.hpp"
#include "storage/ducklake_storage.hpp"
#include "functions/ducklake_table_functions.hpp"
#include "duckdb/main/extension_util.hpp"

namespace duckdb {

static void LoadInternal(DatabaseInstance &instance) {
	ExtensionUtil::RegisterExtension(instance, "ducklake", {"Adds support for DuckLake, SQL as a Lakehouse Format"});

	auto &config = DBConfig::GetConfig(instance);
	config.storage_extensions["ducklake"] = make_uniq<DuckLakeStorageExtension>();

	DuckLakeSnapshotsFunction snapshots;
	ExtensionUtil::RegisterFunction(instance, snapshots);

	DuckLakeTableInfoFunction table_info;
	ExtensionUtil::RegisterFunction(instance, table_info);

	auto table_insertions = DuckLakeTableInsertionsFunction::GetFunctions();
	ExtensionUtil::RegisterFunction(instance, table_insertions);

	auto table_deletions = DuckLakeTableDeletionsFunction::GetFunctions();
	ExtensionUtil::RegisterFunction(instance, table_deletions);

	DuckLakeMergeAdjacentFilesFunction merge_adjacent_files;
	ExtensionUtil::RegisterFunction(instance, merge_adjacent_files);

	DuckLakeCleanupOldFilesFunction cleanup_old_files;
	ExtensionUtil::RegisterFunction(instance, cleanup_old_files);

	DuckLakeExpireSnapshotsFunction expire_snapshots;
	ExtensionUtil::RegisterFunction(instance, expire_snapshots);

	DuckLakeSetOptionFunction set_options;
	ExtensionUtil::RegisterFunction(instance, set_options);

	auto get_options = DuckLakeGetOptionFunction::GetFunctions();
	ExtensionUtil::RegisterFunction(instance, get_options);

	auto table_changes = DuckLakeTableInsertionsFunction::GetDuckLakeTableChanges();
	ExtensionUtil::RegisterFunction(instance, *table_changes);

	DuckLakeListFilesFunction list_files;
	ExtensionUtil::RegisterFunction(instance, list_files);
}

void DucklakeExtension::Load(DuckDB &db) {
	LoadInternal(*db.instance);
}
std::string DucklakeExtension::Name() {
	return "ducklake";
}

std::string DucklakeExtension::Version() const {
#ifdef EXT_VERSION_DUCKLAKE
	return EXT_VERSION_DUCKLAKE;
#else
	return "";
#endif
}

} // namespace duckdb

extern "C" {

DUCKDB_EXTENSION_API void ducklake_init(duckdb::DatabaseInstance &db) {
	duckdb::DuckDB db_wrapper(db);
	db_wrapper.LoadExtension<duckdb::DucklakeExtension>();
}

DUCKDB_EXTENSION_API const char *ducklake_version() {
	return duckdb::DuckDB::LibraryVersion();
}
}
