/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#include "googletest.h"

#include "filesystemmock.h"
#include "projectmanagermock.h"
#include "projectstoragemock.h"
#include "qmldocumentparsermock.h"
#include "qmltypesparsermock.h"

#include <projectstorage/filestatuscache.h>
#include <projectstorage/projectstorage.h>
#include <projectstorage/projectstorageupdater.h>
#include <projectstorage/sourcepathcache.h>
#include <sqlitedatabase.h>

namespace {

namespace Storage = QmlDesigner::Storage;

using QmlDesigner::FileStatus;
using QmlDesigner::SourceId;
using QmlDesigner::Storage::TypeAccessSemantics;
namespace Storage = QmlDesigner::Storage;
using QmlDesigner::IdPaths;
using QmlDesigner::Storage::Version;

MATCHER_P5(IsStorageType,
           moduleId,
           typeName,
           prototype,
           accessSemantics,
           sourceId,
           std::string(negation ? "isn't " : "is ")
               + PrintToString(Storage::Type{moduleId, typeName, prototype, accessSemantics, sourceId}))
{
    const Storage::Type &type = arg;

    return type.moduleId == moduleId && type.typeName == typeName
           && type.accessSemantics == accessSemantics && type.sourceId == sourceId
           && Storage::ImportedTypeName{prototype} == type.prototype;
}

MATCHER_P3(IsPropertyDeclaration,
           name,
           typeName,
           traits,
           std::string(negation ? "isn't " : "is ")
               + PrintToString(Storage::PropertyDeclaration{name, typeName, traits}))
{
    const Storage::PropertyDeclaration &propertyDeclaration = arg;

    return propertyDeclaration.name == name
           && Storage::ImportedTypeName{typeName} == propertyDeclaration.typeName
           && propertyDeclaration.traits == traits;
}

MATCHER_P3(IsExportedType,
           name,
           majorVersion,
           minorVersion,
           std::string(negation ? "isn't " : "is ")
               + PrintToString(Storage::ExportedType{name,
                                                     Storage::Version{majorVersion, minorVersion}}))
{
    const Storage::ExportedType &type = arg;

    return type.name == name && type.version == Storage::Version{majorVersion, minorVersion};
}

MATCHER_P2(IsModule,
           name,
           sourceId,
           std::string(negation ? "isn't " : "is ") + PrintToString(Storage::Module{name, sourceId}))
{
    const Storage::Module &module = arg;

    return module.name == name && module.sourceId == sourceId;
}

MATCHER_P3(IsFileStatus,
           sourceId,
           size,
           lastModified,
           std::string(negation ? "isn't " : "is ")
               + PrintToString(FileStatus{sourceId, size, lastModified}))
{
    const FileStatus &fileStatus = arg;

    return fileStatus.sourceId == sourceId && fileStatus.size == size
           && fileStatus.lastModified == lastModified;
}

class ProjectStorageUpdater : public testing::Test
{
public:
    ProjectStorageUpdater()
    {
        ON_CALL(fileSystemMock, fileStatus(Eq(qmltypesPathSourceId)))
            .WillByDefault(Return(FileStatus{qmltypesPathSourceId, 21, 421}));
        ON_CALL(projectStorageMock, fetchFileStatus(Eq(qmltypesPathSourceId)))
            .WillByDefault(Return(FileStatus{qmltypesPathSourceId, 2, 421}));

        ON_CALL(fileSystemMock, fileStatus(Eq(qmltypes2PathSourceId)))
            .WillByDefault(Return(FileStatus{qmltypes2PathSourceId, 21, 421}));
        ON_CALL(projectStorageMock, fetchFileStatus(Eq(qmltypes2PathSourceId)))
            .WillByDefault(Return(FileStatus{qmltypes2PathSourceId, 2, 421}));

        ON_CALL(fileSystemMock, fileStatus(Eq(qmlDirPathSourceId)))
            .WillByDefault(Return(FileStatus{qmlDirPathSourceId, 21, 421}));
        ON_CALL(projectStorageMock, fetchFileStatus(Eq(qmlDirPathSourceId)))
            .WillByDefault(Return(FileStatus{qmlDirPathSourceId, 2, 421}));

        ON_CALL(projectStorageMock, fetchSourceDependencieIds(Eq(qmlDirPathSourceId)))
            .WillByDefault(Return(QmlDesigner::SourceIds{qmltypesPathSourceId, qmltypes2PathSourceId}));

        QString qmldir{"module Example\ntypeinfo example.qmltypes\n"};
        ON_CALL(projectManagerMock, qtQmlDirs()).WillByDefault(Return(QStringList{"/path/qmldir"}));
        ON_CALL(fileSystemMock, contentAsQString(Eq(QString("/path/qmldir"))))
            .WillByDefault(Return(qmldir));

        ON_CALL(fileSystemMock, fileStatus(Eq(qmlDocumentSourceId1)))
            .WillByDefault(Return(FileStatus{qmlDocumentSourceId1, 22, 12}));
        ON_CALL(projectStorageMock, fetchFileStatus(Eq(qmlDocumentSourceId1)))
            .WillByDefault(Return(FileStatus{qmlDocumentSourceId1, 22, 2}));
        ON_CALL(fileSystemMock, fileStatus(Eq(qmlDocumentSourceId2)))
            .WillByDefault(Return(FileStatus{qmlDocumentSourceId2, 22, 13}));
        ON_CALL(projectStorageMock, fetchFileStatus(Eq(qmlDocumentSourceId2)))
            .WillByDefault(Return(FileStatus{qmlDocumentSourceId2, 22, 2}));
        ON_CALL(fileSystemMock, fileStatus(Eq(qmlDocumentSourceId3)))
            .WillByDefault(Return(FileStatus{qmlDocumentSourceId3, 22, 14}));
        ON_CALL(projectStorageMock, fetchFileStatus(Eq(qmlDocumentSourceId3)))
            .WillByDefault(Return(FileStatus{qmlDocumentSourceId3, 22, 2}));

        firstType.prototype = Storage::ImportedType{"Object"};
        secondType.prototype = Storage::ImportedType{"Object2"};
        thirdType.prototype = Storage::ImportedType{"Object3"};

        ON_CALL(fileSystemMock, contentAsQString(Eq(QString("/path/First.qml"))))
            .WillByDefault(Return(qmlDocument1));
        ON_CALL(fileSystemMock, contentAsQString(Eq(QString("/path/First.2.qml"))))
            .WillByDefault(Return(qmlDocument2));
        ON_CALL(fileSystemMock, contentAsQString(Eq(QString("/path/Second.qml"))))
            .WillByDefault(Return(qmlDocument3));

        ON_CALL(qmlDocumentParserMock, parse(qmlDocument1, _)).WillByDefault([&](auto, auto &imports) {
            imports.push_back(import1);
            return firstType;
        });
        ON_CALL(qmlDocumentParserMock, parse(qmlDocument2, _)).WillByDefault([&](auto, auto &imports) {
            imports.push_back(import2);
            return secondType;
        });
        ON_CALL(qmlDocumentParserMock, parse(qmlDocument3, _)).WillByDefault([&](auto, auto &imports) {
            imports.push_back(import3);
            return thirdType;
        });
    }

protected:
    NiceMock<ProjectManagerMock> projectManagerMock;
    NiceMock<FileSystemMock> fileSystemMock;
    NiceMock<ProjectStorageMock> projectStorageMock;
    NiceMock<QmlTypesParserMock> qmlTypesParserMock;
    NiceMock<QmlDocumentParserMock> qmlDocumentParserMock;
    QmlDesigner::FileStatusCache fileStatusCache{fileSystemMock};
    Sqlite::Database database{":memory:", Sqlite::JournalMode::Memory};
    QmlDesigner::ProjectStorage<Sqlite::Database> storage{database, database.isInitialized()};
    QmlDesigner::SourcePathCache<QmlDesigner::ProjectStorage<Sqlite::Database>> sourcePathCache{
        storage};
    QmlDesigner::ProjectUpdater updater{projectManagerMock,
                                        fileSystemMock,
                                        projectStorageMock,
                                        fileStatusCache,
                                        sourcePathCache,
                                        qmlDocumentParserMock,
                                        qmlTypesParserMock};
    SourceId objectTypeSourceId{sourcePathCache.sourceId("/path/Object")};

    SourceId qmltypesPathSourceId = sourcePathCache.sourceId("/path/example.qmltypes");
    SourceId qmltypes2PathSourceId = sourcePathCache.sourceId("/path/example2.qmltypes");
    SourceId qmlDirPathSourceId = sourcePathCache.sourceId("/path/qmldir");
    QmlDesigner::ModuleId exampleModuleId{&qmlDirPathSourceId};
    SourceId qmlDocumentSourceId1 = sourcePathCache.sourceId("/path/First.qml");
    SourceId qmlDocumentSourceId2 = sourcePathCache.sourceId("/path/First.2.qml");
    SourceId qmlDocumentSourceId3 = sourcePathCache.sourceId("/path/Second.qml");
    Storage::Type objectType{exampleModuleId,
                             "QObject",
                             Storage::NativeType{},
                             Storage::TypeAccessSemantics::Reference,
                             objectTypeSourceId,
                             {Storage::ExportedType{"Object"}, Storage::ExportedType{"Obj"}}};
    QString qmlDocument1{"First{}"};
    QString qmlDocument2{"Second{}"};
    QString qmlDocument3{"Third{}"};
    Storage::Type firstType;
    Storage::Type secondType;
    Storage::Type thirdType;
    Storage::Import import1{"Qml", Storage::Version{2, 3}, qmlDocumentSourceId1};
    Storage::Import import2{"Qml", Storage::Version{}, qmlDocumentSourceId2};
    Storage::Import import3{"Qml", Storage::Version{2}, qmlDocumentSourceId3};
};

TEST_F(ProjectStorageUpdater, GetContentForQmlDirPathsIfFileStatusIsDifferent)
{
    SourceId qmlDir1PathSourceId = sourcePathCache.sourceId("/path/one/qmldir");
    SourceId qmlDir2PathSourceId = sourcePathCache.sourceId("/path/two/qmldir");
    SourceId qmlDir3PathSourceId = sourcePathCache.sourceId("/path/three/qmldir");
    ON_CALL(projectManagerMock, qtQmlDirs())
        .WillByDefault(
            Return(QStringList{"/path/one/qmldir", "/path/two/qmldir", "/path/three/qmldir"}));
    ON_CALL(fileSystemMock, fileStatus(_)).WillByDefault([](auto sourceId) {
        return FileStatus{sourceId, 21, 421};
    });
    ON_CALL(projectStorageMock, fetchFileStatus(_)).WillByDefault([](auto sourceId) {
        return FileStatus{sourceId, 2, 421};
    });
    ON_CALL(fileSystemMock, fileStatus(Eq(qmlDir3PathSourceId)))
        .WillByDefault(Return(FileStatus{qmlDir3PathSourceId, 21, 421}));
    ON_CALL(projectStorageMock, fetchFileStatus(Eq(qmlDir3PathSourceId)))
        .WillByDefault(Return(FileStatus{qmlDir3PathSourceId, 21, 421}));

    EXPECT_CALL(fileSystemMock, contentAsQString(Eq(QString("/path/one/qmldir"))));
    EXPECT_CALL(fileSystemMock, contentAsQString(Eq(QString("/path/two/qmldir"))));

    updater.update();
}

TEST_F(ProjectStorageUpdater, RequestFileStatusFromFileSystem)
{
    EXPECT_CALL(fileSystemMock, fileStatus(Ne(qmlDirPathSourceId))).Times(AnyNumber());

    EXPECT_CALL(fileSystemMock, fileStatus(Eq(qmlDirPathSourceId)));

    updater.update();
}

TEST_F(ProjectStorageUpdater, GetContentForQmlTypes)
{
    QString qmldir{"module Example\ntypeinfo example.qmltypes\n"};
    EXPECT_CALL(fileSystemMock, contentAsQString(Eq(QString("/path/qmldir"))))
        .WillRepeatedly(Return(qmldir));

    EXPECT_CALL(fileSystemMock, contentAsQString(Eq(QString("/path/example.qmltypes"))));

    updater.update();
}

TEST_F(ProjectStorageUpdater, GetContentForQmlTypesIfProjectStorageFileStatusIsInvalid)
{
    QString qmldir{"module Example\ntypeinfo example.qmltypes\n"};
    EXPECT_CALL(fileSystemMock, contentAsQString(Eq(QString("/path/qmldir"))))
        .WillRepeatedly(Return(qmldir));
    ON_CALL(projectStorageMock, fetchFileStatus(Eq(qmltypesPathSourceId)))
        .WillByDefault(Return(FileStatus{}));

    EXPECT_CALL(fileSystemMock, contentAsQString(Eq(QString("/path/example.qmltypes"))));

    updater.update();
}

TEST_F(ProjectStorageUpdater, DontGetContentForQmlTypesIfFileSystemFileStatusIsInvalid)
{
    QString qmldir{"module Example\ntypeinfo example.qmltypes\n"};
    EXPECT_CALL(fileSystemMock, contentAsQString(Eq(QString("/path/qmldir"))))
        .WillRepeatedly(Return(qmldir));
    ON_CALL(fileSystemMock, fileStatus(Eq(qmltypesPathSourceId))).WillByDefault(Return(FileStatus{}));

    EXPECT_CALL(fileSystemMock, contentAsQString(Eq(QString("/path/example.qmltypes")))).Times(0);

    updater.update();
}

TEST_F(ProjectStorageUpdater, ParseQmlTypes)
{
    QString qmldir{"module Example\ntypeinfo example.qmltypes\ntypeinfo example2.qmltypes\n"};
    ON_CALL(fileSystemMock, contentAsQString(Eq(QString("/path/qmldir")))).WillByDefault(Return(qmldir));
    QString qmltypes{"Module {\ndependencies: []}"};
    QString qmltypes2{"Module {\ndependencies: [foo]}"};
    ON_CALL(fileSystemMock, contentAsQString(Eq(QString("/path/example.qmltypes"))))
        .WillByDefault(Return(qmltypes));
    ON_CALL(fileSystemMock, contentAsQString(Eq(QString("/path/example2.qmltypes"))))
        .WillByDefault(Return(qmltypes2));

    EXPECT_CALL(qmlTypesParserMock, parse(qmltypes, _, _, _));
    EXPECT_CALL(qmlTypesParserMock, parse(qmltypes2, _, _, _));

    updater.update();
}

TEST_F(ProjectStorageUpdater, SynchronizeIsEmptyForNoChange)
{
    ON_CALL(projectStorageMock, fetchFileStatus(Eq(qmltypesPathSourceId)))
        .WillByDefault(Return(FileStatus{qmltypesPathSourceId, 21, 421}));
    ON_CALL(projectStorageMock, fetchFileStatus(Eq(qmltypes2PathSourceId)))
        .WillByDefault(Return(FileStatus{qmltypes2PathSourceId, 21, 421}));
    ON_CALL(projectStorageMock, fetchFileStatus(Eq(qmlDirPathSourceId)))
        .WillByDefault(Return(FileStatus{qmlDirPathSourceId, 21, 421}));

    EXPECT_CALL(projectStorageMock,
                synchronize(IsEmpty(), IsEmpty(), IsEmpty(), IsEmpty(), IsEmpty()));

    updater.update();
}

TEST_F(ProjectStorageUpdater, SynchronizeQmlTypes)
{
    auto qmlDirPathSourceId = sourcePathCache.sourceId("/path/qmldir");
    auto qmltypesPathSourceId = sourcePathCache.sourceId("/path/example.qmltypes");
    Storage::Import import{"Qml", Storage::Version{2, 3}, qmltypesPathSourceId};
    QString qmltypes{"Module {\ndependencies: []}"};
    ON_CALL(fileSystemMock, contentAsQString(Eq(QString("/path/example.qmltypes"))))
        .WillByDefault(Return(qmltypes));
    ON_CALL(qmlTypesParserMock, parse(qmltypes, _, _, _))
        .WillByDefault([&](auto, auto &imports, auto &types, auto &sourceIds) {
            types.push_back(objectType);
            imports.push_back(import);
        });

    EXPECT_CALL(projectStorageMock,
                synchronize(ElementsAre(IsModule("Example", qmlDirPathSourceId)),
                            ElementsAre(import),
                            ElementsAre(Eq(objectType)),
                            UnorderedElementsAre(qmlDirPathSourceId, qmltypesPathSourceId),
                            UnorderedElementsAre(IsFileStatus(qmlDirPathSourceId, 21, 421),
                                                 IsFileStatus(qmltypesPathSourceId, 21, 421))));

    updater.update();
}

TEST_F(ProjectStorageUpdater, SynchronizeQmlTypesAreEmptyIfFileDoesNotChanged)
{
    QString qmltypes{"Module {\ndependencies: []}"};
    ON_CALL(fileSystemMock, contentAsQString(Eq(QString("/path/example.qmltypes"))))
        .WillByDefault(Return(qmltypes));
    ON_CALL(qmlTypesParserMock, parse(qmltypes, _, _, _))
        .WillByDefault([&](auto, auto &imports, auto &types, auto &sourceIds) {
            types.push_back(objectType);
        });
    ON_CALL(fileSystemMock, fileStatus(Eq(qmltypesPathSourceId)))
        .WillByDefault(Return(FileStatus{qmltypesPathSourceId, 2, 421}));
    ON_CALL(fileSystemMock, fileStatus(Eq(qmltypes2PathSourceId)))
        .WillByDefault(Return(FileStatus{qmltypes2PathSourceId, 2, 421}));
    ON_CALL(fileSystemMock, fileStatus(Eq(qmlDirPathSourceId)))
        .WillByDefault(Return(FileStatus{qmlDirPathSourceId, 2, 421}));

    EXPECT_CALL(projectStorageMock,
                synchronize(IsEmpty(), IsEmpty(), IsEmpty(), IsEmpty(), IsEmpty()));

    updater.update();
}

TEST_F(ProjectStorageUpdater, GetContentForQmlDocuments)
{
    QString qmldir{"module Example\nFirstType 1.0 First.qml\nFirstTypeV2 2.2 "
                   "First.2.qml\nSecondType 2.1 OldSecond.qml\nSecondType 2.2 Second.qml\n"};
    EXPECT_CALL(fileSystemMock, contentAsQString(Eq(QString("/path/qmldir"))))
        .WillRepeatedly(Return(qmldir));

    EXPECT_CALL(fileSystemMock, contentAsQString(Eq(QString("/path/First.qml"))));
    EXPECT_CALL(fileSystemMock, contentAsQString(Eq(QString("/path/First.2.qml"))));
    EXPECT_CALL(fileSystemMock, contentAsQString(Eq(QString("/path/Second.qml"))));

    updater.update();
}

TEST_F(ProjectStorageUpdater, ParseQmlDocuments)
{
    QString qmldir{"module Example\nFirstType 1.0 First.qml\nFirstTypeV2 2.2 "
                   "First.2.qml\nSecondType 2.1 OldSecond.qml\nSecondType 2.2 Second.qml\n"};
    QString qmlDocument1{"First{}"};
    QString qmlDocument2{"Second{}"};
    QString qmlDocument3{"Third{}"};
    ON_CALL(fileSystemMock, contentAsQString(Eq(QString("/path/qmldir")))).WillByDefault(Return(qmldir));
    ON_CALL(fileSystemMock, contentAsQString(Eq(QString("/path/First.qml"))))
        .WillByDefault(Return(qmlDocument1));
    ON_CALL(fileSystemMock, contentAsQString(Eq(QString("/path/First.2.qml"))))
        .WillByDefault(Return(qmlDocument2));
    ON_CALL(fileSystemMock, contentAsQString(Eq(QString("/path/Second.qml"))))
        .WillByDefault(Return(qmlDocument3));

    EXPECT_CALL(qmlDocumentParserMock, parse(qmlDocument1, _));
    EXPECT_CALL(qmlDocumentParserMock, parse(qmlDocument2, _));
    EXPECT_CALL(qmlDocumentParserMock, parse(qmlDocument3, _));

    updater.update();
}

TEST_F(ProjectStorageUpdater, SynchronizeQmlDocuments)
{
    QString qmldir{"module Example\nFirstType 1.0 First.qml\nFirstType 2.2 "
                   "First.2.qml\nSecondType 2.1 OldSecond.qml\nSecondType 2.2 Second.qml\n"};
    ON_CALL(fileSystemMock, contentAsQString(Eq(QString("/path/qmldir")))).WillByDefault(Return(qmldir));

    EXPECT_CALL(projectStorageMock,
                synchronize(ElementsAre(IsModule("Example", qmlDirPathSourceId)),
                            UnorderedElementsAre(import1, import2, import3),
                            UnorderedElementsAre(
                                AllOf(IsStorageType(exampleModuleId,
                                                    "First.qml",
                                                    Storage::ImportedType{"Object"},
                                                    TypeAccessSemantics::Reference,
                                                    qmlDocumentSourceId1),
                                      Field(&Storage::Type::exportedTypes,
                                            ElementsAre(IsExportedType("FirstType", 1, 0)))),
                                AllOf(IsStorageType(exampleModuleId,
                                                    "First.2.qml",
                                                    Storage::ImportedType{"Object2"},
                                                    TypeAccessSemantics::Reference,
                                                    qmlDocumentSourceId2),
                                      Field(&Storage::Type::exportedTypes,
                                            ElementsAre(IsExportedType("FirstType", 2, 2)))),
                                AllOf(IsStorageType(exampleModuleId,
                                                    "Second.qml",
                                                    Storage::ImportedType{"Object3"},
                                                    TypeAccessSemantics::Reference,
                                                    qmlDocumentSourceId3),
                                      Field(&Storage::Type::exportedTypes,
                                            ElementsAre(IsExportedType("SecondType", 2, 2))))),
                            UnorderedElementsAre(qmlDirPathSourceId,
                                                 qmlDocumentSourceId1,
                                                 qmlDocumentSourceId2,
                                                 qmlDocumentSourceId3),
                            UnorderedElementsAre(IsFileStatus(qmlDirPathSourceId, 21, 421),
                                                 IsFileStatus(qmlDocumentSourceId1, 22, 12),
                                                 IsFileStatus(qmlDocumentSourceId2, 22, 13),
                                                 IsFileStatus(qmlDocumentSourceId3, 22, 14))));

    updater.update();
}

TEST_F(ProjectStorageUpdater, SynchronizeQmlDocumentsDontUpdateIfUpToDate)
{
    QString qmldir{"module Example\nFirstType 1.0 First.qml\nFirstType 2.2 "
                   "First.2.qml\nSecondType 2.1 OldSecond.qml\nSecondType 2.2 Second.qml\n"};
    ON_CALL(fileSystemMock, contentAsQString(Eq(QString("/path/qmldir")))).WillByDefault(Return(qmldir));
    ON_CALL(projectStorageMock, fetchFileStatus(Eq(qmlDocumentSourceId3)))
        .WillByDefault(Return(FileStatus{qmlDocumentSourceId3, 22, 14}));

    EXPECT_CALL(
        projectStorageMock,
        synchronize(ElementsAre(IsModule("Example", qmlDirPathSourceId)),
                    UnorderedElementsAre(import1, import2),
                    UnorderedElementsAre(AllOf(IsStorageType(exampleModuleId,
                                                             "First.qml",
                                                             Storage::ImportedType{"Object"},
                                                             TypeAccessSemantics::Reference,
                                                             qmlDocumentSourceId1),
                                               Field(&Storage::Type::exportedTypes,
                                                     ElementsAre(IsExportedType("FirstType", 1, 0)))),
                                         AllOf(IsStorageType(exampleModuleId,
                                                             "First.2.qml",
                                                             Storage::ImportedType{"Object2"},
                                                             TypeAccessSemantics::Reference,
                                                             qmlDocumentSourceId2),
                                               Field(&Storage::Type::exportedTypes,
                                                     ElementsAre(IsExportedType("FirstType", 2, 2))))),
                    UnorderedElementsAre(qmlDirPathSourceId, qmlDocumentSourceId1, qmlDocumentSourceId2),
                    UnorderedElementsAre(IsFileStatus(qmlDirPathSourceId, 21, 421),
                                         IsFileStatus(qmlDocumentSourceId1, 22, 12),
                                         IsFileStatus(qmlDocumentSourceId2, 22, 13))));

    updater.update();
}

TEST_F(ProjectStorageUpdater, SynchronizeModules)
{
    SourceId qmlDirPathSourceId2 = sourcePathCache.sourceId("/path2/qmldir");
    ON_CALL(fileSystemMock, fileStatus(Eq(qmlDirPathSourceId2)))
        .WillByDefault(Return(FileStatus{qmlDirPathSourceId2, 22, 423}));
    ON_CALL(projectStorageMock, fetchFileStatus(Eq(qmlDirPathSourceId2)))
        .WillByDefault(Return(FileStatus{qmlDirPathSourceId2, 2, 421}));
    QString qmldir2{"module Example2\n"};
    ON_CALL(projectManagerMock, qtQmlDirs())
        .WillByDefault(Return(QStringList{"/path/qmldir", "/path2/qmldir"}));
    ON_CALL(fileSystemMock, contentAsQString(Eq(QString("/path2/qmldir")))).WillByDefault(Return(qmldir2));

    EXPECT_CALL(projectStorageMock,
                synchronize(UnorderedElementsAre(IsModule("Example", qmlDirPathSourceId),
                                                 IsModule("Example2", qmlDirPathSourceId2)),
                            _,
                            _,
                            _,
                            UnorderedElementsAre(IsFileStatus(qmlDirPathSourceId, 21, 421),
                                                 IsFileStatus(qmltypesPathSourceId, 21, 421),
                                                 IsFileStatus(qmlDirPathSourceId2, 22, 423))));

    updater.update();
}

TEST_F(ProjectStorageUpdater, UpdateQmldirDocuments)
{
    QString qmldir{"module Example\nFirstType 1.1 First.qml\nFirstType 2.2 "
                   "First.2.qml\nSecondType 2.1 OldSecond.qml\nSecondType 2.2 Second.qml\n"};
    ON_CALL(fileSystemMock, contentAsQString(Eq(QString("/path/qmldir")))).WillByDefault(Return(qmldir));
    ON_CALL(projectStorageMock, fetchFileStatus(Eq(qmlDocumentSourceId3)))
        .WillByDefault(Return(FileStatus{qmlDocumentSourceId3, 22, 14}));

    updater.pathsWithIdsChanged({});
}

} // namespace