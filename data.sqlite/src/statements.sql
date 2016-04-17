SELECT_TIME SELECT cast(strftime('%s', 'now') as integer)
SELECT_SCHEMA SELECT group_concat("sql",';') FROM "sqlite_master" WHERE "name" LIKE 'Z_'
SELECT_TABLE_NAMES SELECT "name" FROM "sqlite_master" WHERE "name" LIKE 'Z_'

COUNT_CORPS_BY_NUM SELECT count(*) FROM "ZA" NATURAL JOIN "ZT" WHERE "corpNum" = ? AND "typeName" = 'corporation'
COUNT_ACTORS_BY_NAME SELECT count(*) FROM "ZA" NATURAL JOIN "ZT" WHERE "actorName" = ? AND "typeName" = ?
SELECT_CORP_BY_CORP_NUM SELECT "actor_" FROM "ZA" NATURAL JOIN "ZT" WHERE "corpNum" = ? AND "typeName" = 'corporation'
SELECT_ACTOR_BY_NAME_AND_TYPE SELECT "actor_" FROM "ZA" NATURAL JOIN "ZT" WHERE "actorName" = ? AND "typeName" = ?
INSERT_ACTOR INSERT INTO "ZA" ("actorName", "type_", "corpNum", "actor$") values(?, (SELECT "type_" from "ZT" where "typeName" = ?), ?, ?)

COUNT_INFO_BY_KEY_AND_ACTOR SELECT count(*) FROM "ZI" WHERE "infoKey"=? AND "actor_"=?
COUNT_INFO_BY_KEY SELECT count(*) FROM "ZI" WHERE "infoKey"=?
SELECT_INFO_VALUE_BY_KEY SELECT "infoValue" FROM "ZI" WHERE "infoKey"=?
INSERT_INFO INSERT INTO "ZI" ("actor_", "infoKey", "infoValue") VALUES(?, ?, ?)
REPLACE_INFO INSERT OR REPLACE INTO "ZI" ("infoKey", "infoValue", "info$") VALUES (?, ?, cast(strftime('%s', 'now') as integer))
UPDATE_INFO_1 UPDATE "ZI" SET "infoValue" = ?, "info$" = ? WHERE "actor_" = ? AND "infoKey" = 'rank'
UPDATE_INFO_2 UPDATE "ZI" SET "infoValue" = ?, "info$" = cast(strftime('%s', 'now') as integer) WHERE "infoKey" = ? AND "actor_" = ?

COUNT_ONLINE_BY_ACTOR SELECT count(*) FROM "ZO" WHERE "actor_" = ?
COUNT_ONLINE_BY_TYPE SELECT count(*) FROM "ZO" NATURAL JOIN "ZA" NATURAL JOIN "ZT" WHERE "isOnline" = 1 AND "typeName" = ? AND "online2$" = (SELECT "online2$" from ZO ORDER BY "online2$" DESC LIMIT 1 )
SELECT_ONLINE_BY_ID SELECT "isOnline", "online_" FROM "ZO" WHERE "actor_" = ?1 AND "online2$" = (SELECT "online2$" FROM "ZO" where "actor_" = ?1 ORDER BY "online2$" DESC LIMIT 1)
SELECT_OFFLINE_ACTORS SELECT "online_", "actor_", "isOnline" FROM (SELECT "online_", "actor_", "isOnline", max("online2$") AS "m" FROM "ZO" GROUP BY "actor_") where "m" < ?
INSERT_ONLINE_ENTRY INSERT INTO "ZO" ("actor_", "isOnline", "online1$", "online2$") VALUES (?1, 0, ?2, ?2)
UPDATE_ONLINE_STAMP_BY_ID UPDATE "ZO" SET "online2$" = ? WHERE "online_" = ?

SELECT_WARP_CIM SELECT "sector_", group_concat("warp",' ') from "ZW" group by "sector_"
INSERT_NEW_WARP INSERT INTO "ZW" ("sector_", "warp") VALUES(?, ?)
