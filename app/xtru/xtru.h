/*
 *
 * Copyright (C) 2023 SuitableApp
 *
 * This file is part of Extreme Unloader(XTRU).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

#define ROWID_STR_LEN                     (18+1) ///< Length of the ROWID when represented by string.
#define COLUMN_NAME_LEN                  (120+1) ///< Length of column name.
#define PARTITIONED_LEN                    (3+1)
#define FLAG_LEN                           (1+1)
#define PRIVILEGE_LEN                     (40+1)
#define IOT_TYPE_LEN                      (12+1)
#define BUFFER_POOL_LEN                    (7+1)
#define DEGREE_LEN                         (7+1)
#define INSTANCES_LEN                      (7+1)
#define GENERATED_LEN                      (1+1)

#define  MINIMUM_COLUMN_LENGTH               32  ///< Column name width (meta.sql)

#include <nsApp.h>
#include <cAppConf.h>
#include <cFeature.h>
#include <copydd/cCopyDd.h>
#include <cBootstrap.h>
#include <copydd/cTargetOwners.h>
#include <copydd/cTargetTables.h>
#include <copydd/cAllUsers.h>
#include <copydd/cAllTables.h>
#include <copydd/cAllIndexes.h>
#include <copydd/cAllTriggers.h>
#include <copydd/cAllMviews.h>
#include <copydd/cAllExternalTables.h>
#include <copydd/cColumnOptionalities.h>
#include <copydd/cAllConstraints.h>
#include <copydd/cAllTabColumns.h>
#include <copydd/cAllConsColumns.h>
#include <copydd/cAllObjects.h>
#include <copydd/cUserObjects.h>
#include <copydd/cAllTabPartitions.h>
#include <copydd/cAllPartTables.h>
#include <copydd/cRowidRanges.h>
#include <copydd/cReposRefresher.h>
#include <copydd/cEffectiveCons.h>
#include <copydd/cReferenceGraphTab.h>
#include <copydd/cReferenceGraphObj.h>
#include <copydd/cAllTabPrivs.h>
#include <copydd/cSessionPrivs.h>

#include <getmeta/cPrivsGiven.h>
#include <getmeta/cScriptMaker.h>
#include <getmeta/cConsValidate.h>

#include <getdata/cStartValues.h>
#include <getdata/cUnload.h>
#include <getdata/cQuery.h>
#include <getdata/cPartitionedByScheme.h>
#include <getdata/cPartitionedByRowid.h>

#include <getmeta/cNumObjs.h>
#include <getmeta/cDescriber.h>
#include <getmeta/nsGetMeta.h>
#include <getmeta/cGetMeta.h>
#include <getmeta/cStartValues.h>
#include <getmeta/cFunctionalReplace.h>
#include <getmeta/cAllTabPrivs.h>
#include <getmeta/cUserObjAuditOpts.h>
#include <getmeta/cUserAuditObject.h>
#include <getmeta/cUserDbLinks.h>
#include <getmeta/cAllSequences.h>
#include <getmeta/cAllSource.h>
#include <getmeta/cAllTabComments.h>
#include <getmeta/cAllColComments.h>
#include <getmeta/cUserSegments.h>
#include <getmeta/cAllConstraints.h>
#include <getmeta/cKeyColumns.h>
#include <getmeta/cDataDefault.h>
#include <getmeta/cAllTabColumns.h>
#include <getmeta/cAllSubPartKeyColumns.h>
#include <getmeta/cAllPartKeyColumns.h>
#include <getmeta/cCompressObject.h>
#include <getmeta/cAllTabSubPartitions.h>
#include <getmeta/cAllSubPartitionTemplates.h>
#include <getmeta/cAllTabPartitions.h>
#include <getmeta/cAllPartTables.h>
#include <getmeta/cAllTables.h>
#include <getmeta/cAllIndExpressions.h>
#include <getmeta/cAllIndColumns.h>
#include <getmeta/cAllIndSubPartitions.h>
#include <getmeta/cAllIndPartitions.h>
#include <getmeta/cAllPartIndexes.h>
#include <getmeta/cUserIndexes.h>
#include <getmeta/cHashExpr.h>
#include <getmeta/cAllClusters.h>
#include <getmeta/cUserCluColumns.h>
#include <getmeta/cEffectiveKeyCons.h>
#include <getmeta/cParentWithChild.h>
#include <getmeta/cAllConsColumns.h>
#include <getmeta/cConsOfCheck.h>
#include <getmeta/cConsOfFKey.h>
#include <getmeta/cConsOfAKey.h>
#include <getmeta/cAllLobs.h>
#include <getmeta/cAllTriggers.h>
#include <getmeta/cDbaMviewLogFilterCols.h>
#include <getmeta/cAllMviewLogs.h>
#include <getmeta/cAllExternalLocations.h>
#include <getmeta/cAllExternalTables.h>
#include <getmeta/cAllLogGroupColumns.h>
#include <getmeta/cAllLogGroups.h>
#include <getmeta/cOutputOrderTbl.h>
#include <getmeta/cSqlFmt.h>
#include <getmeta/cAllMviews.h>
#include <getmeta/cTablesWithFk.h>
#include <getmeta/cAllRefreshChildren.h>
#include <getmeta/cAllRefresh.h>
#include <getmeta/cAllViews.h>
#include <getmeta/cAllSynonyms.h>
#include <getmeta/cUserJobs.h>

#include <getmeta/cAllObjects.h>

