
#include <tuple>
#include <gtest/gtest.h>

#include <fstream>
#include <cstring>

#include "FastIni.h"


//#define SHOW_OUTPUT

#ifdef SHOW_OUTPUT

#include <iostream>

#endif // SHOW_OUTPUT

using namespace ::testing;

using DAttribute =  std::pair<std::string, std::string>;

class CTestSection
{
public:

  CTestSection(){}

  CTestSection(const std::string& aName, const std::vector<DAttribute>& aValues)
    : mName{aName}, mValues{aValues}
  {}

  const std::string& getName() const
  {
    return mName;
  }

  const std::vector<DAttribute>& getValues() const
  {
    return mValues;
  }

private:
  std::string mName;
  std::vector<DAttribute> mValues;

};

using DSectionsAttributesBufferSize = std::tuple<unsigned int, unsigned int, unsigned int, unsigned int>;

unsigned int getNumberOfSections(const DSectionsAttributesBufferSize& aParam)
{
  return std::get<0>(aParam);
}

unsigned int getNumberOfAttributes(const DSectionsAttributesBufferSize& aParam)
{
  return std::get<1>(aParam);
}

unsigned int getBufferSize(const DSectionsAttributesBufferSize& aParam)
{
  return std::get<2>(aParam);
}

unsigned int getCommentSize(const DSectionsAttributesBufferSize& aParam)
{
  return std::get<3>(aParam);
}

class CCurrentRunData
{
  enum class EItemType
  {
    SECTION,
    KEY,
    VALUE
  };

  static DSectionsAttributesBufferSize mParams;
  static std::vector<CTestSection> mSections;
  static std::vector<CTestSection>::const_iterator mSectionsIt;
  static std::vector<DAttribute>::const_iterator mAttributesIt;
  static EItemType mNextItemType;

public:

  class CItems
  {
  public:
    CItems(const std::string* aSection, const std::string* aKey, const std::string* aValue)
      : mSection{aSection}
      , mKey{aKey}
      , mValue{aValue}
    {}

    const std::string* getSection() const
    {
      return mSection;
    }

    const std::string* getKey() const
    {
      return mKey;
    }

    const std::string* getValue() const
    {
      return mValue;
    }

  private:
    const std::string* mSection;
    const std::string* mKey;
    const std::string* mValue;
  };

  static CItems getItems()
  {
    if (mNextItemType == EItemType::SECTION)
    {
      mNextItemType = EItemType::KEY;
      const std::string* section = &mSectionsIt->getName();
      mAttributesIt = mSectionsIt->getValues().begin();
      return CItems{section, nullptr, nullptr};
    }
    else if (mNextItemType == EItemType::KEY)
    {
      mNextItemType = EItemType::VALUE;
      const std::string* key = &mAttributesIt->first;
      return CItems{nullptr, key, nullptr};
    }
    else if (mNextItemType == EItemType::VALUE)
    {
      const std::string* value = &mAttributesIt->second;
      ++mAttributesIt;
      if (mAttributesIt == mSectionsIt->getValues().end())
      {
        mNextItemType = EItemType::SECTION;
        ++mSectionsIt;
      }
      else
      {
        mNextItemType = EItemType::KEY;
      }
      return CItems{nullptr, nullptr, value};;
    }
    return CItems{nullptr, nullptr, nullptr};
  }

  static void setSections(const std::vector<CTestSection>& aSections)
  {
    mSections = aSections;
    mSectionsIt = mSections.begin();
    mNextItemType = EItemType::SECTION;
  }

  static void setParam(const DSectionsAttributesBufferSize& aParam)
  {
    mParams = aParam;
  }

  static DSectionsAttributesBufferSize& getParam()
  {
    return mParams;
  }

};

DSectionsAttributesBufferSize CCurrentRunData::mParams;
std::vector<CTestSection> CCurrentRunData::mSections;
std::vector<CTestSection>::const_iterator CCurrentRunData::mSectionsIt;
std::vector<DAttribute>::const_iterator CCurrentRunData::mAttributesIt;
CCurrentRunData::EItemType CCurrentRunData::mNextItemType;


class CFastIniUnitTest : public TestWithParam<DSectionsAttributesBufferSize>
{
public:

  void SetUp()
  {
    const auto& param = GetParam();
    const int comment = getCommentSize(param);
    CCurrentRunData::setParam(param);
    std::ofstream file(sFilePath);

    std::vector<CTestSection> sections;
    sections.reserve(getNumberOfSections(param));
    for (unsigned int sectionIndex = 0; sectionIndex < getNumberOfSections(param); ++sectionIndex)
    {
      const std::string section{std::string{"section"} + std::to_string(sectionIndex)};
      file<<"["<<section<<"]\n";

      std::vector<DAttribute> attributes;
      attributes.reserve(getNumberOfAttributes(param));

      for (unsigned int attributeIndex = 0; attributeIndex < getNumberOfAttributes(param); ++attributeIndex)
      {
        const std::string key{std::string{"key"} + std::to_string(attributeIndex)};
        const std::string value{std::string{"value"} + std::to_string(attributeIndex)};
        attributes.emplace_back(key, value);
        if (comment > 0)
        {
          file<<"; ";
          for (int c = 0; c < comment; ++c)
          {
            file<<'c';
          }
          file<<'\n';
        }
        file<<key<<"="<<value<<"\n";
      }
      sections.emplace_back(section, attributes);
      file<<"\n";
    }
    file.close();
    CCurrentRunData::setSections(sections);
  }


protected:

  static const char* sFilePath;
};

const char* CFastIniUnitTest::sFilePath = "test.ini";

bool predicate(const std::string* aExpected, const char* aText, int aTextSize)
{
  if (aExpected == nullptr && aText == nullptr)
  {
    return true;
  }
  else if (aExpected == nullptr)
  {
    return false;
  }
  else if (aText == nullptr)
  {
    return false;
  }
  const std::size_t size = aExpected->size();
  if (size != static_cast<std::size_t>(aTextSize))
  {
    SCOPED_TRACE(std::string{"Incorrect value. Expected"}
                 + *aExpected
                 + " received: "
                 + std::string(aText, static_cast<std::size_t>(aTextSize)));
    return false;
  }
  if (memcmp(aExpected->c_str(), aText, size) != 0)
  {
    SCOPED_TRACE(std::string{"Incorrect value. Expected"}
                 + *aExpected
                 + " received: "
                 + std::string(aText, static_cast<std::size_t>(aTextSize)));
    return false;
  }
  return true;
}

int parseListener(char* aSection, int aSectionSize, char* aKey, int aKeySize, char* aValue, int aValueSize)
{
  const auto expectedItems = CCurrentRunData::getItems();
  const std::string* sectionExpected = expectedItems.getSection();
  const std::string* keyExpected = expectedItems.getKey();
  const std::string* valueExpected = expectedItems.getValue();

#ifdef SHOW_OUTPUT
  std::cout<<"Section: "
           <<(aSection ? std::string(aSection, aSectionSize) : "null")
           <<" Key: "
           <<(aKey ? std::string(aKey, aKeySize) : "null")
           <<" Value: "
           <<(aValue ? std::string(aValue, aValueSize) : "null")
           <<std::endl;
#endif // SHOW_OUTPUT

  if (!predicate(sectionExpected, aSection, aSectionSize))
  {
    return -1;
  }
  if (!predicate(keyExpected, aKey, aKeySize))
  {
    return -1;
  }
  if (!predicate(valueExpected, aValue, aValueSize))
  {
    return -1;
  }
  return 0;
}

TEST_P(CFastIniUnitTest, testParam)
{

  std::vector<char> buffer;
  const auto bufferSize = getBufferSize(CCurrentRunData::getParam());
  buffer.resize(bufferSize);
  const auto result = fastIniParse(sFilePath, buffer.data(), bufferSize, parseListener);
  ASSERT_EQ(result, 0);

}

std::string testNameFormatter(const TestParamInfo<DSectionsAttributesBufferSize>& aParam)
{
  const auto& param = aParam.param;
  return std::string{"Section_"} + std::to_string(getNumberOfSections(param))
    + "_Attributes_" + std::to_string(getNumberOfAttributes(param))
    + "_BufferSize_" + std::to_string(getBufferSize(param))
    + "_Comment_"+std::to_string(getCommentSize(param));
}

INSTANTIATE_TEST_CASE_P(NoEndingSpace,
                        CFastIniUnitTest,
                        Combine(
                          Range(1u, 10u, 1u),
                          Range(1u, 10u, 1u),
                          Range(12u, 128u, 1u),
                          Range(1u, 10u, 1u)
                        ),
                        testNameFormatter);


