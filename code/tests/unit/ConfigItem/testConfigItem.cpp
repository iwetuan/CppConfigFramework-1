/* This file is part of C++ Config Framework.
 *
 * C++ Config Framework is free software: you can redistribute it and/or modify it under the terms
 * of the GNU Lesser General Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * C++ Config Framework is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with C++ Config
 * Framework. If not, see <http://www.gnu.org/licenses/>.
 */

/*!
 * \file
 *
 * Contains unit tests for ConfigItem class
 */

// C++ Config Framework includes
#include <CppConfigFramework/ConfigItem.hpp>
#include <CppConfigFramework/ConfigReader.hpp>

// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QJsonDocument>
#include <QtCore/QLine>
#include <QtTest/QTest>

// System includes

// Forward declarations

// Macros

// Test config classes -----------------------------------------------------------------------------

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
template<>
struct std::hash<QString> {
    std::size_t operator()(const QString &v) const noexcept
    {
        return qHash(v);
    }
};
#endif

using namespace CppConfigFramework;

class TestRequiredConfigParameter : public ConfigItem
{
public:
    int param = 0;

private:
    bool loadConfigParameters(const ConfigObjectNode &config) override
    {
        return loadRequiredConfigParameter(&param,
                                           "param",
                                           config,
                                           makeConfigParameterRangeValidator(-50, 50));
    }

    bool storeConfigParameters(ConfigObjectNode *config) override
    {
        return storeConfigParameter(param, "param", config);
    }

    QString validateConfig() const override
    {
        if (param > 20)
        {
            return "validateConfig error";
        }

        return QString();
    }
};

class TestRequiredConfigInvalidParameter : public ConfigItem
{
public:
    int param = 0;

private:
    bool loadConfigParameters(const ConfigObjectNode &config) override
    {
        return loadRequiredConfigParameter(&param, "0param", config);
    }

    bool storeConfigParameters(ConfigObjectNode *config) override
    {
        return storeConfigParameter(param, "0param", config);
    }
};

class TestOptionalConfigParameter : public ConfigItem
{
public:
    int param = 0;

private:
    bool loadConfigParameters(const ConfigObjectNode &config) override
    {
        bool loaded = false;
        return loadOptionalConfigParameter(&param,
                                           "param",
                                           config,
                                           makeConfigParameterRangeValidator(-50, 50),
                                           &loaded);
    }

    bool storeConfigParameters(ConfigObjectNode *config) override
    {
        return storeConfigParameter(param, "param", config);
    }
};

class TestOptionalConfigInvalidParameter : public ConfigItem
{
public:
    int param = 0;

private:
    bool loadConfigParameters(const ConfigObjectNode &config) override
    {
        bool loaded = false;
        return loadOptionalConfigParameter(&param, "0param", config, &loaded);
    }

    bool storeConfigParameters(ConfigObjectNode *config) override
    {
        return storeConfigParameter(param, "0param", config);
    }
};

class TestConfigContainerItem : public ConfigItem
{
public:
    TestConfigContainerItem(const QString &name = QString(), const int param = 0)
        : name(name),
          param(param)
    {
    }

    QString name;
    int param = 0;

private:
    bool loadConfigParameters(const ConfigObjectNode &config) override
    {
        return loadRequiredConfigParameter(&param,
                                           "param",
                                           config,
                                           makeConfigParameterRangeValidator(-50, 50));
    }

    bool storeConfigParameters(ConfigObjectNode *config) override
    {
        return storeConfigParameter(param, "param", config);
    }
};

template<typename T>
class TestRequiredConfigContainer : public ConfigItem
{
public:
    T container;

private:
    static TestConfigContainerItem createItem(const QString &name)
    {
        return TestConfigContainerItem(name);
    }

    bool loadConfigParameters(const ConfigObjectNode &config) override
    {
        return loadRequiredConfigContainer(&container,
                                           "container",
                                           config,
                                           createItem);
    }

    bool storeConfigParameters(ConfigObjectNode *config) override
    {
        return storeConfigContainer(container, "container", config);
    }
};

template<typename T>
class TestOptionalConfigContainer : public ConfigItem
{
public:
    T container;

private:
    static TestConfigContainerItem createItem(const QString &name)
    {
        return TestConfigContainerItem(name);
    }

    bool loadConfigParameters(const ConfigObjectNode &config) override
    {
        bool loaded = false;
        return loadOptionalConfigContainer(&container, "container", config, createItem, &loaded);
    }

    bool storeConfigParameters(ConfigObjectNode *config) override
    {
        return storeConfigContainer(container, "container", config);
    }
};

class TestRequiredConfigContainerInvalidParameter : public ConfigItem
{
public:
    QList<TestConfigContainerItem> container;

private:
    bool loadConfigParameters(const ConfigObjectNode &config) override
    {
        return loadRequiredConfigContainer(&container, "0container", config);
    }

    bool storeConfigParameters(ConfigObjectNode *config) override
    {
        return storeConfigContainer(container, "0container", config);
    }
};

class TestOptionalConfigContainerInvalidParameter : public ConfigItem
{
public:
    QList<TestConfigContainerItem> container;

private:
    bool loadConfigParameters(const ConfigObjectNode &config) override
    {
        bool loaded = false;
        return loadOptionalConfigContainer(&container, "0container", config, &loaded);
    }

    bool storeConfigParameters(ConfigObjectNode *config) override
    {
        return storeConfigContainer(container, "0container", config);
    }
};

using ConfigItemPtr = std::shared_ptr<ConfigItem>;
Q_DECLARE_METATYPE(ConfigItemPtr)

// Test class declaration --------------------------------------------------------------------------

class TestConfigItem : public QObject
{
    Q_OBJECT

private slots:
    // Functions executed by QtTest before and after test suite
    void initTestCase();
    void cleanupTestCase();

    // Functions executed by QtTest before and after each test
    void init();
    void cleanup();

    // Test functions
    void testLoadConfigAtPath();
    void testLoadConfigAtPath_data();

    void testLoadOptionalConfigAtPath();
    void testLoadOptionalConfigAtPath_data();

    void testLoadConfig();
    void testLoadConfig_data();

    void testLoadOptionalConfig();
    void testLoadOptionalConfig_data();

    void testLoadConfigParameter();

    void testLoadConfigContainer();

    void testStoreConfigAtPath();
    void testStoreConfigAtPath_data();

    void testStoreConfigAtPathNegativeTests();

    void testStoreConfig();

    void testStoreConfigContainer();
};

// Test Case init/cleanup methods ------------------------------------------------------------------

void TestConfigItem::initTestCase()
{
}

void TestConfigItem::cleanupTestCase()
{
}

// Test init/cleanup methods -----------------------------------------------------------------------

void TestConfigItem::init()
{
}

void TestConfigItem::cleanup()
{
}

// Test: loadConfigAtPath() method -----------------------------------------------------------------

void TestConfigItem::testLoadConfigAtPath()
{
    QFETCH(QString, path);
    QFETCH(bool, expectedResult);
    QFETCH(int, expectedResultValue);

    // Read config file
    const QString configFilePath(QStringLiteral(":/TestData/TestLoadConfigAtPath.json"));
    auto environmentVariables = EnvironmentVariables::loadFromProcess();
    ConfigReader configReader;

    auto config = configReader.read(configFilePath,
                                    QDir::current(),
                                    ConfigNodePath::ROOT_PATH,
                                    ConfigNodePath::ROOT_PATH,
                                    std::vector<const ConfigObjectNode *>(),
                                    &environmentVariables);
    QVERIFY(config);

    TestRequiredConfigParameter configStructure;
    QCOMPARE(configStructure.loadConfigAtPath(path, *config), expectedResult);
    QCOMPARE(configStructure.param, expectedResultValue);
}

void TestConfigItem::testLoadConfigAtPath_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<bool>("expectedResult");
    QTest::addColumn<int>("expectedResultValue");

    QTest::newRow("Absolute path exists") << "/level1/level2/actualConfig" << true << 1;
    QTest::newRow("Relative path exists") << "level1/level2/actualConfig" << true << 1;

    QTest::newRow("Absolute path: missing") << "/level1/level2/missingConfig" << false << 0;
    QTest::newRow("Relative path: missing") << "level1/level2/missingConfig" << false << 0;

    QTest::newRow("Absolute path: invalid") << "/level1/level2/123" << false << 0;
    QTest::newRow("Relative path: invalid") << "level1/level2/123" << false << 0;

    QTest::newRow("Invalid config node") << "/level1/level2/invalidConfigNode" << false << 0;
    QTest::newRow("Invalid config parameter value range")
            << "/level1/level2/configWithInvalidParamValueRange" << false << -100;
}

// Test: loadOptionalConfigAtPath() method ---------------------------------------------------------

void TestConfigItem::testLoadOptionalConfigAtPath()
{
    QFETCH(QString, path);
    QFETCH(bool, expectedResult);
    QFETCH(bool, expectedResultLoaded);
    QFETCH(int, expectedResultValue);

    // Read config file
    const QString configFilePath(QStringLiteral(":/TestData/TestLoadConfigAtPath.json"));
    auto environmentVariables = EnvironmentVariables::loadFromProcess();
    ConfigReader configReader;

    auto config = configReader.read(configFilePath,
                                    QDir::current(),
                                    ConfigNodePath::ROOT_PATH,
                                    ConfigNodePath::ROOT_PATH,
                                    std::vector<const ConfigObjectNode *>(),
                                    &environmentVariables);
    QVERIFY(config);

    TestRequiredConfigParameter configStructure;
    bool loaded = false;
    QCOMPARE(configStructure.loadOptionalConfigAtPath(path, *config, &loaded), expectedResult);
    QCOMPARE(loaded, expectedResultLoaded);
    QCOMPARE(configStructure.param, expectedResultValue);
}

void TestConfigItem::testLoadOptionalConfigAtPath_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<bool>("expectedResult");
    QTest::addColumn<bool>("expectedResultLoaded");
    QTest::addColumn<int>("expectedResultValue");

    QTest::newRow("Absolute path: exists") << "/level1/level2/actualConfig" << true << true << 1;
    QTest::newRow("Relative path: exists") << "level1/level2/actualConfig" << true << true << 1;

    QTest::newRow("Absolute path: missing") << "/level1/level2/missingConfig" << true << false << 0;
    QTest::newRow("Relative path: missing") << "level1/level2/missingConfig" << true << false << 0;

    QTest::newRow("Absolute path: invalid") << "/level1/level2/123" << false << false << 0;
    QTest::newRow("Relative path: invalid") << "level1/level2/123" << false << false << 0;

    QTest::newRow("Invalid config node")
            << "/level1/level2/invalidConfigNode" << false << false << 0;
    QTest::newRow("Invalid config parameter value range")
            << "/level1/level2/configWithInvalidParamValueRange" << false << false << -100;
}

// Test: loadConfig() method -----------------------------------------------------------------------

void TestConfigItem::testLoadConfig()
{
    QFETCH(QString, parameterName);
    QFETCH(bool, expectedResult);
    QFETCH(int, expectedResultValue);

    // Read config file
    const QString configFilePath(QStringLiteral(":/TestData/TestLoadConfig.json"));
    auto environmentVariables = EnvironmentVariables::loadFromProcess();
    ConfigReader configReader;

    auto config = configReader.read(configFilePath,
                                    QDir::current(),
                                    ConfigNodePath::ROOT_PATH,
                                    ConfigNodePath::ROOT_PATH,
                                    std::vector<const ConfigObjectNode *>(),
                                    &environmentVariables);
    QVERIFY(config);

    TestRequiredConfigParameter configStructure;
    QCOMPARE(configStructure.loadConfig(parameterName, *config), expectedResult);

    QCOMPARE(configStructure.param, expectedResultValue);
}

void TestConfigItem::testLoadConfig_data()
{
    QTest::addColumn<QString>("parameterName");
    QTest::addColumn<bool>("expectedResult");
    QTest::addColumn<int>("expectedResultValue");

    QTest::newRow("Config exists") << "actualConfig" << true << 1;
    QTest::newRow("Missing config") << "missingConfig"  << false << 0;
    QTest::newRow("Invalid config") << "123" << false << 0;
    QTest::newRow("Invalid config node") << "invalidConfigNode" << false << 0;
    QTest::newRow("Invalid config parameter value range")
            << "configWithInvalidParamValueRange" << false << -100;
    QTest::newRow("Invalid config parameter value")
            << "configWithInvalidParamValue" << false << 30;
}

// Test: loadOptionalConfig() method ---------------------------------------------------------------

void TestConfigItem::testLoadOptionalConfig()
{
    QFETCH(QString, parameterName);
    QFETCH(bool, expectedResult);
    QFETCH(bool, expectedResultLoaded);
    QFETCH(int, expectedResultValue);

    // Read config file
    const QString configFilePath(QStringLiteral(":/TestData/TestLoadConfig.json"));
    auto environmentVariables = EnvironmentVariables::loadFromProcess();
    ConfigReader configReader;

    auto config = configReader.read(configFilePath,
                                    QDir::current(),
                                    ConfigNodePath::ROOT_PATH,
                                    ConfigNodePath::ROOT_PATH,
                                    std::vector<const ConfigObjectNode *>(),
                                    &environmentVariables);
    QVERIFY(config);

    TestRequiredConfigParameter configStructure;
    bool loaded = false;
    QCOMPARE(configStructure.loadOptionalConfig(parameterName, *config, &loaded), expectedResult);
    QCOMPARE(loaded, expectedResultLoaded);
    QCOMPARE(configStructure.param, expectedResultValue);
}

void TestConfigItem::testLoadOptionalConfig_data()
{
    QTest::addColumn<QString>("parameterName");
    QTest::addColumn<bool>("expectedResult");
    QTest::addColumn<bool>("expectedResultLoaded");
    QTest::addColumn<int>("expectedResultValue");

    QTest::newRow("Config exists") << "actualConfig" << true << true << 1;
    QTest::newRow("Missing config") << "missingConfig" << true << false << 0;
    QTest::newRow("Invalid config") << "123" << false << false << 0;
    QTest::newRow("Invalid config node") << "invalidConfigNode" << false << false << 0;
    QTest::newRow("Invalid config parameter value range")
            << "configWithInvalidParamValueRange" << false << false << -100;
}

// Test: loading of required and optional config parameters ----------------------------------------

void TestConfigItem::testLoadConfigParameter()
{
    // Read config file
    const QString configFilePath(QStringLiteral(":/TestData/LoadConfigParameter.json"));
    auto environmentVariables = EnvironmentVariables::loadFromProcess();
    ConfigReader configReader;

    auto config = configReader.read(configFilePath,
                                    QDir::current(),
                                    ConfigNodePath::ROOT_PATH,
                                    ConfigNodePath::ROOT_PATH,
                                    std::vector<const ConfigObjectNode *>(),
                                    &environmentVariables);
    QVERIFY(config);

    // Load config with "param" parameter
    {
        TestRequiredConfigParameter required;
        TestOptionalConfigParameter optional;

        QCOMPARE(required.loadConfig("configWithParam", *config), true);
        QCOMPARE(optional.loadConfig("configWithParam", *config), true);

        QCOMPARE(required.param, 1);
        QCOMPARE(optional.param, 1);
    }

    // Load config without "param" parameter
    {
        TestRequiredConfigParameter required;
        TestOptionalConfigParameter optional;

        QCOMPARE(required.loadConfig("configWithoutParam", *config), false);
        QCOMPARE(optional.loadConfig("configWithoutParam", *config), true);

        QCOMPARE(required.param, 0);
        QCOMPARE(optional.param, 0);

        // Make sure that the values stay unchanged
        required.param = 11;
        optional.param = 11;

        QCOMPARE(required.loadConfig("configWithoutParam", *config), false);
        QCOMPARE(optional.loadConfig("configWithoutParam", *config), true);

        QCOMPARE(required.param, 11);
        QCOMPARE(optional.param, 11);
    }

    // Test loading parameters with invalid name
    {
        TestRequiredConfigInvalidParameter required;
        TestOptionalConfigInvalidParameter optional;

        QCOMPARE(required.loadConfig("configWithParam", *config), false);
        QCOMPARE(optional.loadConfig("configWithParam", *config), false);

        QCOMPARE(required.param, 0);
        QCOMPARE(optional.param, 0);
    }

    // Load config with "param" parameter of invalid node type
    {
        TestRequiredConfigParameter required;
        TestOptionalConfigParameter optional;

        QCOMPARE(required.loadConfig("configWithInvalidNodeType", *config), false);
        QCOMPARE(optional.loadConfig("configWithInvalidNodeType", *config), false);

        QCOMPARE(required.param, 0);
        QCOMPARE(optional.param, 0);
    }

    // Load config with "param" parameter of invalid data type
    {
        TestRequiredConfigParameter required;
        TestOptionalConfigParameter optional;

        QCOMPARE(required.loadConfig("configWithInvalidParam", *config), false);
        QCOMPARE(optional.loadConfig("configWithInvalidParam", *config), false);

        QCOMPARE(required.param, 0);
        QCOMPARE(optional.param, 0);
    }
}

// Test: loading of required and optional config containers ----------------------------------------

void TestConfigItem::testLoadConfigContainer()
{
    // Read config file
    const QString configFilePath(QStringLiteral(":/TestData/LoadConfigContainer.json"));
    auto environmentVariables = EnvironmentVariables::loadFromProcess();
    ConfigReader configReader;

    auto config = configReader.read(configFilePath,
                                    QDir::current(),
                                    ConfigNodePath::ROOT_PATH,
                                    ConfigNodePath::ROOT_PATH,
                                    std::vector<const ConfigObjectNode *>(),
                                    &environmentVariables);
    QVERIFY(config);

    QMap<QString, int> expected =
    {
        { "aaa", 1 },
        { "bbb", 2 },
        { "ccc", 3 }
    };

    // Qt containers -------------------------------------------------------------------------------

    // QVector
    {
        TestRequiredConfigContainer<QVector<TestConfigContainerItem>> required;
        TestOptionalConfigContainer<QVector<TestConfigContainerItem>> optional;

        QCOMPARE(required.loadConfig("actualConfig", *config), true);
        QCOMPARE(optional.loadConfig("actualConfig", *config), true);

        QCOMPARE(required.container.size(), 3);
        QCOMPARE(optional.container.size(), 3);

        auto itRequired = required.container.begin();
        auto itOptional = optional.container.begin();

        QCOMPARE(itRequired->param, expected.value(itRequired->name));
        QCOMPARE(itOptional->param, expected.value(itOptional->name));
    }

    // QList
    {
        TestRequiredConfigContainer<QList<TestConfigContainerItem>> required;
        TestOptionalConfigContainer<QList<TestConfigContainerItem>> optional;

        QCOMPARE(required.loadConfig("actualConfig", *config), true);
        QCOMPARE(optional.loadConfig("actualConfig", *config), true);

        QCOMPARE(required.container.size(), 3);
        QCOMPARE(optional.container.size(), 3);

        auto itRequired = required.container.begin();
        auto itOptional = optional.container.begin();

        QCOMPARE(itRequired->param, expected.value(itRequired->name));
        QCOMPARE(itOptional->param, expected.value(itOptional->name));
    }

    // QMap
    {
        TestRequiredConfigContainer<QMap<QString, TestConfigContainerItem>> required;
        TestOptionalConfigContainer<QMap<QString, TestConfigContainerItem>> optional;

        QCOMPARE(required.loadConfig("actualConfig", *config), true);
        QCOMPARE(optional.loadConfig("actualConfig", *config), true);

        QCOMPARE(required.container.size(), 3);
        QCOMPARE(optional.container.size(), 3);

        auto itRequired = required.container.begin();
        auto itOptional = optional.container.begin();

        QCOMPARE(itRequired->param, expected.value(itRequired.key()));
        QCOMPARE(itOptional->param, expected.value(itOptional.key()));
    }

    // QHash
    {
        TestRequiredConfigContainer<QHash<QString, TestConfigContainerItem>> required;
        TestOptionalConfigContainer<QHash<QString, TestConfigContainerItem>> optional;

        QCOMPARE(required.loadConfig("actualConfig", *config), true);
        QCOMPARE(optional.loadConfig("actualConfig", *config), true);

        QCOMPARE(required.container.size(), 3);
        QCOMPARE(optional.container.size(), 3);

        auto itRequired = required.container.begin();
        auto itOptional = optional.container.begin();

        QCOMPARE(itRequired->param, expected.value(itRequired.key()));
        QCOMPARE(itOptional->param, expected.value(itOptional.key()));
    }

    // STL containers ------------------------------------------------------------------------------

    // std::vector
    {
        TestRequiredConfigContainer<std::vector<TestConfigContainerItem>> required;
        TestOptionalConfigContainer<std::vector<TestConfigContainerItem>> optional;

        QCOMPARE(required.loadConfig("actualConfig", *config), true);
        QCOMPARE(optional.loadConfig("actualConfig", *config), true);

        QCOMPARE(required.container.size(), static_cast<size_t>(3));
        QCOMPARE(optional.container.size(), static_cast<size_t>(3));

        auto itRequired = required.container.begin();
        auto itOptional = optional.container.begin();

        QCOMPARE(itRequired->param, expected.value(itRequired->name));
        QCOMPARE(itOptional->param, expected.value(itOptional->name));
    }

    // std::list
    {
        TestRequiredConfigContainer<std::list<TestConfigContainerItem>> required;
        TestOptionalConfigContainer<std::list<TestConfigContainerItem>> optional;

        QCOMPARE(required.loadConfig("actualConfig", *config), true);
        QCOMPARE(optional.loadConfig("actualConfig", *config), true);

        QCOMPARE(required.container.size(), static_cast<size_t>(3));
        QCOMPARE(optional.container.size(), static_cast<size_t>(3));

        auto itRequired = required.container.begin();
        auto itOptional = optional.container.begin();

        QCOMPARE(itRequired->param, expected.value(itRequired->name));
        QCOMPARE(itOptional->param, expected.value(itOptional->name));
    }

    // std::map
    {
        TestRequiredConfigContainer<std::map<QString, TestConfigContainerItem>> required;
        TestOptionalConfigContainer<std::map<QString, TestConfigContainerItem>> optional;

        QCOMPARE(required.loadConfig("actualConfig", *config), true);
        QCOMPARE(optional.loadConfig("actualConfig", *config), true);

        QCOMPARE(required.container.size(), static_cast<size_t>(3));
        QCOMPARE(optional.container.size(), static_cast<size_t>(3));

        auto itRequired = required.container.begin();
        auto itOptional = optional.container.begin();

        QCOMPARE(itRequired->second.param, expected.value(itRequired->first));
        QCOMPARE(itOptional->second.param, expected.value(itOptional->first));
    }

    // std::unordered_map
    {
        TestRequiredConfigContainer<std::unordered_map<QString, TestConfigContainerItem>> required;
        TestOptionalConfigContainer<std::unordered_map<QString, TestConfigContainerItem>> optional;

        QCOMPARE(required.loadConfig("actualConfig", *config), true);
        QCOMPARE(optional.loadConfig("actualConfig", *config), true);

        QCOMPARE(required.container.size(), static_cast<size_t>(3));
        QCOMPARE(optional.container.size(), static_cast<size_t>(3));

        auto itRequired = required.container.begin();
        auto itOptional = optional.container.begin();

        QCOMPARE(itRequired->second.param, expected.value(itRequired->first));
        QCOMPARE(itOptional->second.param, expected.value(itOptional->first));
    }

    // Negative tests ------------------------------------------------------------------------------

    // Invalid parameter name
    {
        TestRequiredConfigContainerInvalidParameter requiredSequential;
        TestOptionalConfigContainerInvalidParameter optionalSequential;

        TestRequiredConfigContainerInvalidParameter requiredAssociative;
        TestOptionalConfigContainerInvalidParameter optionalAssociative;

        QCOMPARE(requiredSequential.loadConfig("actualConfig", *config), false);
        QCOMPARE(optionalSequential.loadConfig("actualConfig", *config), false);

        QCOMPARE(requiredAssociative.loadConfig("actualConfig", *config), false);
        QCOMPARE(optionalAssociative.loadConfig("actualConfig", *config), false);

        QCOMPARE(requiredSequential.container.size(), 0);
        QCOMPARE(optionalSequential.container.size(), 0);

        QCOMPARE(requiredAssociative.container.size(), 0);
        QCOMPARE(optionalAssociative.container.size(), 0);
    }

    // Missing parameter
    {
        TestRequiredConfigContainer<QList<TestConfigContainerItem>> requiredSequential;
        TestOptionalConfigContainer<QList<TestConfigContainerItem>> optionalSequential;

        TestRequiredConfigContainer<QMap<QString, TestConfigContainerItem>> requiredAssociative;
        TestOptionalConfigContainer<QMap<QString, TestConfigContainerItem>> optionalAssociative;

        QCOMPARE(requiredSequential.loadConfig("missingConfig", *config), false);
        QCOMPARE(optionalSequential.loadConfig("missingConfig", *config), true);

        QCOMPARE(requiredAssociative.loadConfig("missingConfig", *config), false);
        QCOMPARE(optionalAssociative.loadConfig("missingConfig", *config), true);

        QCOMPARE(requiredSequential.container.size(), 0);
        QCOMPARE(optionalSequential.container.size(), 0);

        QCOMPARE(requiredAssociative.container.size(), 0);
        QCOMPARE(optionalAssociative.container.size(), 0);
    }

    // Invalid parameter node type
    {
        TestRequiredConfigContainer<QList<TestConfigContainerItem>> requiredSequential;
        TestOptionalConfigContainer<QList<TestConfigContainerItem>> optionalSequential;

        TestRequiredConfigContainer<QMap<QString, TestConfigContainerItem>> requiredAssociative;
        TestOptionalConfigContainer<QMap<QString, TestConfigContainerItem>> optionalAssociative;

        QCOMPARE(requiredSequential.loadConfig("configWithInvalidNodeType", *config), false);
        QCOMPARE(optionalSequential.loadConfig("configWithInvalidNodeType", *config), false);

        QCOMPARE(requiredAssociative.loadConfig("configWithInvalidNodeType", *config), false);
        QCOMPARE(optionalAssociative.loadConfig("configWithInvalidNodeType", *config), false);

        QCOMPARE(requiredSequential.container.size(), 0);
        QCOMPARE(optionalSequential.container.size(), 0);

        QCOMPARE(requiredAssociative.container.size(), 0);
        QCOMPARE(optionalAssociative.container.size(), 0);
    }

    // Invalid parameter item
    {
        TestRequiredConfigContainer<QList<TestConfigContainerItem>> requiredSequential;
        TestOptionalConfigContainer<QList<TestConfigContainerItem>> optionalSequential;

        TestRequiredConfigContainer<QMap<QString, TestConfigContainerItem>> requiredAssociative;
        TestOptionalConfigContainer<QMap<QString, TestConfigContainerItem>> optionalAssociative;

        QCOMPARE(requiredSequential.loadConfig("configWithInvalidItem", *config), false);
        QCOMPARE(optionalSequential.loadConfig("configWithInvalidItem", *config), false);

        QCOMPARE(requiredAssociative.loadConfig("configWithInvalidItem", *config), false);
        QCOMPARE(optionalAssociative.loadConfig("configWithInvalidItem", *config), false);
    }
}

// Test: storeConfigAtPath() method ----------------------------------------------------------------

void TestConfigItem::testStoreConfigAtPath()
{
    QFETCH(ConfigItemPtr, configItem);
    QFETCH(QString, path);
    QFETCH(bool, withValueNode);
    QFETCH(QJsonObject, expected);

    ConfigObjectNode root;

    if (withValueNode)
    {
        root.setMember("value", ConfigValueNode(""));
    }

    QVERIFY(configItem->storeConfigAtPath(path, &root));

    auto result = ConfigWriter::writeToJsonConfig(root);
    QCOMPARE(result.toJson(), QJsonDocument(expected).toJson());
}

void TestConfigItem::testStoreConfigAtPath_data()
{
    QTest::addColumn<ConfigItemPtr>("configItem");
    QTest::addColumn<QString>("path");
    QTest::addColumn<bool>("withValueNode");
    QTest::addColumn<QJsonObject>("expected");

    {
        auto configItem = std::make_shared<TestRequiredConfigParameter>();
        configItem->param = 1;

        QJsonObject expected
        {
            {
                "config", QJsonObject
                {
                    {
                        "test" , QJsonObject
                        {
                            { "#param", 1 }
                        }
                    }
                }
            }
        };

        QTest::newRow("sub-node") << static_cast<ConfigItemPtr>(configItem)
                                  << "test"
                                  << false
                                  << expected;
    }

    {
        auto configItem = std::make_shared<TestRequiredConfigParameter>();
        configItem->param = 123;

        QJsonObject expected
        {
            {
                "config", QJsonObject
                {
                    {
                        "test1" , QJsonObject
                        {
                            {
                                "test2" , QJsonObject
                                {
                                    { "#param", 123 }
                                }
                            }
                        }
                    }
                }
            }
        };

        QTest::newRow("sub-sub-node") << static_cast<ConfigItemPtr>(configItem)
                                      << "test1/test2"
                                      << false
                                      << expected;
    }

    {
        auto configItem = std::make_shared<TestRequiredConfigParameter>();
        configItem->param = 1;

        QJsonObject expected
        {
            {
                "config", QJsonObject
                {
                    { "#value" , "" },
                    {
                        "test" , QJsonObject
                        {
                            { "#param", 1 }
                        }
                    }
                }
            }
        };

        QTest::newRow("with parent reference") << static_cast<ConfigItemPtr>(configItem)
                                               << "value/../test"
                                               << true
                                               << expected;
    }
}

// Test: negative tests for storeConfigAtPath() method ---------------------------------------------

void TestConfigItem::testStoreConfigAtPathNegativeTests()
{
    ConfigObjectNode root;
    root.setMember("value", ConfigValueNode(1));

    TestRequiredConfigParameter configItem;
    configItem.param = 1;

    QVERIFY(!configItem.storeConfigAtPath("path", nullptr));
    QVERIFY(!configItem.storeConfigAtPath("", &root));
    QVERIFY(!configItem.storeConfigAtPath("0path", &root));
    QVERIFY(!configItem.storeConfigAtPath("value", &root));
    QVERIFY(!configItem.storeConfigAtPath("value/path", &root));
}

// Test: tests for storeConfig() methods -----------------------------------------------------------

void TestConfigItem::testStoreConfig()
{
    ConfigObjectNode root;

    TestRequiredConfigParameter configItem;
    configItem.param = 1;

    // storeConfig(config)
    QVERIFY(!configItem.storeConfig(nullptr));
    QVERIFY(configItem.storeConfig(&root));

    QCOMPARE(root.count(), 1);
    QVERIFY(root.contains("param"));
    QVERIFY(root.member("param")->isValue());
    QCOMPARE(root.member("param")->toValue().value(), QJsonValue(1));

    TestRequiredConfigInvalidParameter invalidConfigItem;
    invalidConfigItem.param = 1;

    root.removeAll();
    QVERIFY(!invalidConfigItem.storeConfig(&root));

    // storeConfig(parameterName, config)
    root.removeAll();

    QVERIFY(!configItem.storeConfig("", &root));
    QVERIFY(!configItem.storeConfig("0test", &root));
    QVERIFY(configItem.storeConfig("test", &root));

    const auto *node = root.nodeAtPath("test/param");
    QVERIFY(node != nullptr);
    QVERIFY(node->isValue());
    QCOMPARE(node->toValue().value(), QJsonValue(1));
}

// Test: tests for storeConfigContainer() method ---------------------------------------------------

void TestConfigItem::testStoreConfigContainer()
{
    std::map<QString, int> expected =
    {
        { "aaa", 1 },
        { "bbb", 2 },
        { "ccc", 3 }
    };

    // Qt containers -------------------------------------------------------------------------------

    // QVector
    {
        ConfigObjectNode root;

        TestRequiredConfigContainer<QVector<TestConfigContainerItem>> configItem;

        for (const auto &item : expected)
        {
            configItem.container.append(TestConfigContainerItem(item.first, item.second));
        }

        QVERIFY(configItem.storeConfig(&root));

        QCOMPARE(root.count(), 1);
        QVERIFY(root.contains("container"));

        const auto *node = root.member("container");
        QVERIFY(node != nullptr);
        QVERIFY(node->isObject());

        const auto &containerNode = node->toObject();
        QCOMPARE(containerNode.count(), 3);

        int index = 0;
        for (const auto &item : expected)
        {
            const QString name = "Item" + QString::number(index);
            const int value = item.second;

            QVERIFY(containerNode.contains(name));

            const auto *itemNode = containerNode.member(name);
            QVERIFY(itemNode != nullptr);
            QVERIFY(itemNode->isObject());

            const auto &itemObject = itemNode->toObject();
            QCOMPARE(itemObject.count(), 1);

            const auto *paramNode = itemObject.member("param");
            QVERIFY(paramNode != nullptr);
            QVERIFY(paramNode->isValue());

            const auto &paramValue = paramNode->toValue();
            QCOMPARE(paramValue.value(), QJsonValue(value));

            index++;
        }
    }

    // QList
    {
        ConfigObjectNode root;

        TestRequiredConfigContainer<QList<TestConfigContainerItem>> configItem;

        for (const auto &item : expected)
        {
            configItem.container.append(TestConfigContainerItem(item.first, item.second));
        }

        QVERIFY(configItem.storeConfig(&root));

        QCOMPARE(root.count(), 1);
        QVERIFY(root.contains("container"));

        const auto *node = root.member("container");
        QVERIFY(node != nullptr);
        QVERIFY(node->isObject());

        const auto &containerNode = node->toObject();
        QCOMPARE(containerNode.count(), 3);

        int index = 0;
        for (const auto &item : expected)
        {
            const QString name = "Item" + QString::number(index);
            const int value = item.second;

            QVERIFY(containerNode.contains(name));

            const auto *itemNode = containerNode.member(name);
            QVERIFY(itemNode != nullptr);
            QVERIFY(itemNode->isObject());

            const auto &itemObject = itemNode->toObject();
            QCOMPARE(itemObject.count(), 1);

            const auto *paramNode = itemObject.member("param");
            QVERIFY(paramNode != nullptr);
            QVERIFY(paramNode->isValue());

            const auto &paramValue = paramNode->toValue();
			QCOMPARE(paramValue.value(), QJsonValue(value));

            index++;
        }
    }

    // QMap
    {
        ConfigObjectNode root;

        TestRequiredConfigContainer<QMap<QString, TestConfigContainerItem>> configItem;

        for (const auto &item : expected)
        {
            configItem.container.insert(item.first,
                                        TestConfigContainerItem(item.first, item.second));
        }

        QVERIFY(configItem.storeConfig(&root));

        QCOMPARE(root.count(), 1);
        QVERIFY(root.contains("container"));

        const auto *node = root.member("container");
        QVERIFY(node != nullptr);
        QVERIFY(node->isObject());

        const auto &containerNode = node->toObject();
        QCOMPARE(containerNode.count(), 3);

        for (const auto &item : expected)
        {
            const QString &name = item.first;
            const int value = item.second;

            QVERIFY(containerNode.contains(name));

            const auto *itemNode = containerNode.member(name);
            QVERIFY(itemNode != nullptr);
            QVERIFY(itemNode->isObject());

            const auto &itemObject = itemNode->toObject();
            QCOMPARE(itemObject.count(), 1);

            const auto *paramNode = itemObject.member("param");
            QVERIFY(paramNode != nullptr);
            QVERIFY(paramNode->isValue());

            const auto &paramValue = paramNode->toValue();
			QCOMPARE(paramValue.value(), QJsonValue(value));
        }
    }

    // QHash
    {
        ConfigObjectNode root;

        TestRequiredConfigContainer<QHash<QString, TestConfigContainerItem>> configItem;

        for (const auto &item : expected)
        {
            configItem.container.insert(item.first,
                                        TestConfigContainerItem(item.first, item.second));
        }

        QVERIFY(configItem.storeConfig(&root));

        QCOMPARE(root.count(), 1);
        QVERIFY(root.contains("container"));

        const auto *node = root.member("container");
        QVERIFY(node != nullptr);
        QVERIFY(node->isObject());

        const auto &containerNode = node->toObject();
        QCOMPARE(containerNode.count(), 3);

        for (const auto &item : expected)
        {
            const QString &name = item.first;
            const int value = item.second;

            QVERIFY(containerNode.contains(name));

            const auto *itemNode = containerNode.member(name);
            QVERIFY(itemNode != nullptr);
            QVERIFY(itemNode->isObject());

            const auto &itemObject = itemNode->toObject();
            QCOMPARE(itemObject.count(), 1);

            const auto *paramNode = itemObject.member("param");
            QVERIFY(paramNode != nullptr);
            QVERIFY(paramNode->isValue());

            const auto &paramValue = paramNode->toValue();
			QCOMPARE(paramValue.value(), QJsonValue(value));
        }
    }

    // STL containers ------------------------------------------------------------------------------

    // std::vector
    {
        ConfigObjectNode root;

        TestRequiredConfigContainer<std::vector<TestConfigContainerItem>> configItem;

        for (const auto &item : expected)
        {
            configItem.container.emplace_back(item.first, item.second);
        }

        QVERIFY(configItem.storeConfig(&root));

        QCOMPARE(root.count(), 1);
        QVERIFY(root.contains("container"));

        const auto *node = root.member("container");
        QVERIFY(node != nullptr);
        QVERIFY(node->isObject());

        const auto &containerNode = node->toObject();
        QCOMPARE(containerNode.count(), 3);

        int index = 0;
        for (const auto &item : expected)
        {
            const QString name = "Item" + QString::number(index);
            const int value = item.second;

            QVERIFY(containerNode.contains(name));

            const auto *itemNode = containerNode.member(name);
            QVERIFY(itemNode != nullptr);
            QVERIFY(itemNode->isObject());

            const auto &itemObject = itemNode->toObject();
            QCOMPARE(itemObject.count(), 1);

            const auto *paramNode = itemObject.member("param");
            QVERIFY(paramNode != nullptr);
            QVERIFY(paramNode->isValue());

            const auto &paramValue = paramNode->toValue();
			QCOMPARE(paramValue.value(), QJsonValue(value));

            index++;
        }
    }

    // std::list
    {
        ConfigObjectNode root;

        TestRequiredConfigContainer<std::list<TestConfigContainerItem>> configItem;

        for (const auto &item : expected)
        {
            configItem.container.emplace_back(item.first, item.second);
        }

        QVERIFY(configItem.storeConfig(&root));

        QCOMPARE(root.count(), 1);
        QVERIFY(root.contains("container"));

        const auto *node = root.member("container");
        QVERIFY(node != nullptr);
        QVERIFY(node->isObject());

        const auto &containerNode = node->toObject();
        QCOMPARE(containerNode.count(), 3);

        int index = 0;
        for (const auto &item : expected)
        {
            const QString name = "Item" + QString::number(index);
            const int value = item.second;

            QVERIFY(containerNode.contains(name));

            const auto *itemNode = containerNode.member(name);
            QVERIFY(itemNode != nullptr);
            QVERIFY(itemNode->isObject());

            const auto &itemObject = itemNode->toObject();
            QCOMPARE(itemObject.count(), 1);

            const auto *paramNode = itemObject.member("param");
            QVERIFY(paramNode != nullptr);
            QVERIFY(paramNode->isValue());

            const auto &paramValue = paramNode->toValue();
			QCOMPARE(paramValue.value(), QJsonValue(value));

            index++;
        }
    }

    // std::map
    {
        ConfigObjectNode root;

        TestRequiredConfigContainer<std::map<QString, TestConfigContainerItem>> configItem;

        for (const auto &item : expected)
        {
            configItem.container.emplace(item.first,
                                         TestConfigContainerItem(item.first, item.second));
        }

        QVERIFY(configItem.storeConfig(&root));

        QCOMPARE(root.count(), 1);
        QVERIFY(root.contains("container"));

        const auto *node = root.member("container");
        QVERIFY(node != nullptr);
        QVERIFY(node->isObject());

        const auto &containerNode = node->toObject();
        QCOMPARE(containerNode.count(), 3);

        for (const auto &item : expected)
        {
            const QString &name = item.first;
            const int value = item.second;

            QVERIFY(containerNode.contains(name));

            const auto *itemNode = containerNode.member(name);
            QVERIFY(itemNode != nullptr);
            QVERIFY(itemNode->isObject());

            const auto &itemObject = itemNode->toObject();
            QCOMPARE(itemObject.count(), 1);

            const auto *paramNode = itemObject.member("param");
            QVERIFY(paramNode != nullptr);
            QVERIFY(paramNode->isValue());

            const auto &paramValue = paramNode->toValue();
			QCOMPARE(paramValue.value(), QJsonValue(value));
        }
    }

    // std::unordered_map
    {
        ConfigObjectNode root;

        TestRequiredConfigContainer<std::unordered_map<QString, TestConfigContainerItem>> configItem;

        for (const auto &item : expected)
        {
            configItem.container.emplace(item.first,
                                         TestConfigContainerItem(item.first, item.second));
        }

        QVERIFY(configItem.storeConfig(&root));

        QCOMPARE(root.count(), 1);
        QVERIFY(root.contains("container"));

        const auto *node = root.member("container");
        QVERIFY(node != nullptr);
        QVERIFY(node->isObject());

        const auto &containerNode = node->toObject();
        QCOMPARE(containerNode.count(), 3);

        for (const auto &item : expected)
        {
            const QString &name = item.first;
            const int value = item.second;

            QVERIFY(containerNode.contains(name));

            const auto *itemNode = containerNode.member(name);
            QVERIFY(itemNode != nullptr);
            QVERIFY(itemNode->isObject());

            const auto &itemObject = itemNode->toObject();
            QCOMPARE(itemObject.count(), 1);

            const auto *paramNode = itemObject.member("param");
            QVERIFY(paramNode != nullptr);
            QVERIFY(paramNode->isValue());

            const auto &paramValue = paramNode->toValue();
			QCOMPARE(paramValue.value(), QJsonValue(value));
        }
    }

    // Negative tests ------------------------------------------------------------------------------

    // Invalid parameter name
    {
        ConfigObjectNode root;

        TestRequiredConfigContainerInvalidParameter configItem;

        for (const auto &item : expected)
        {
            configItem.container.append(TestConfigContainerItem(item.first, item.second));
        }

        QVERIFY(!configItem.storeConfig(&root));

        QCOMPARE(root.count(), 0);
    }
}

// Main function -----------------------------------------------------------------------------------

QTEST_MAIN(TestConfigItem)
#include "testConfigItem.moc"
