/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_REFERENCE_MANAGER_H_
#define SRC_GENIE_CORE_REFERENCE_MANAGER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <algorithm>
#include <deque>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/meta/reference.h"
#include "genie/core/reference-collection.h"
#include "genie/util/make-unique.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core {

/**
 * @brief
 */
class ReferenceManager {
 private:
    ReferenceCollection mgr;  //!< @brief

    struct CacheLine {
        std::shared_ptr<const std::string> chunk;  //!< @brief
        std::weak_ptr<const std::string> memory;   //!< @brief
        std::mutex loadMutex;                      //!< @brief
    };

    std::map<std::string, std::vector<std::unique_ptr<CacheLine>>> data;  //!< @brief
    std::map<size_t, std::string> indices;
    std::deque<std::pair<std::string, size_t>> cacheInfo;  //!< @brief
    std::mutex cacheInfoLock;                              //!< @brief
    uint64_t cacheSize;                                    //!< @brief
    static const uint64_t CHUNK_SIZE;                      //!< @brief

    /**
     * @brief
     * @param name
     * @param num
     */
    void touch(const std::string& name, size_t num);

 public:
    /**
     * @brief
     * @param id
     */
    void validateRefID(size_t id);

    /**
     * @brief
     * @param ref
     * @return
     */
    size_t ref2ID(const std::string& ref);

    /**
     * @brief
     * @param id
     * @return
     */
    std::string ID2Ref(size_t id);

    /**
     * @brief
     * @param id
     * @return
     */
    bool refKnown(size_t id);

    /**
     * @brief
     * @return
     */
    static size_t getChunkSize();

    /**
     * @brief
     */
    struct ReferenceExcerpt {
     private:
        std::string ref_name;                                  //!< @brief
        size_t global_start;                                   //!< @brief
        size_t global_end;                                     //!< @brief
        std::vector<std::shared_ptr<const std::string>> data;  //!< @brief

     public:
        /**
         * @brief
         * @return
         */
        [[nodiscard]] bool isEmpty() const;

        /**
         * @brief
         * @param e
         */
        void merge(ReferenceExcerpt& e);

        /**
         * @brief
         * @return
         */
        [[nodiscard]] size_t getGlobalStart() const;

        /**
         * @brief
         * @return
         */
        [[nodiscard]] size_t getGlobalEnd() const;

        /**
         * @brief
         * @return
         */
        [[nodiscard]] const std::string& getRefName() const;

        /**
         * @brief
         * @param pos
         * @return
         */
        [[nodiscard]] std::shared_ptr<const std::string> getChunkAt(size_t pos) const;

        /**
         * @brief
         * @param pos
         * @param dat
         */
        void mapChunkAt(size_t pos, std::shared_ptr<const std::string> dat);

        /**
         * @brief
         */
        ReferenceExcerpt();

        /**
         * @brief
         * @param name
         * @param start
         * @param end
         */
        ReferenceExcerpt(std::string name, size_t start, size_t end);

        /**
         * @brief
         * @return
         */
        static const std::shared_ptr<const std::string>& undef_page();

        /**
         * @brief
         * @param newEnd
         */
        void extend(size_t newEnd);

        /**
         * @brief
         * @param page
         * @return
         */
        static bool isMapped(const std::shared_ptr<const std::string>& page);

        /**
         * @brief
         * @param pos
         * @return
         */
        [[nodiscard]] bool isMapped(size_t pos) const;

        /**
         * @brief
         */
        struct Stepper {
         private:
            std::vector<std::shared_ptr<const std::string>>::const_iterator startVecIt;  //!< @brief
            std::vector<std::shared_ptr<const std::string>>::const_iterator vecIt;       //!< @brief
            std::vector<std::shared_ptr<const std::string>>::const_iterator endVecIt;    //!< @brief
            size_t stringPos;                                                            //!< @brief
            const char* curString;                                                       //!< @brief

         public:
            /**
             * @brief
             * @param e
             */
            explicit Stepper(const ReferenceExcerpt& e);

            /**
             * @brief
             * @param off
             */
            void inc(size_t off = 1);

            /**
             * @brief
             * @return
             */
            [[nodiscard]] bool end() const;

            /**
             * @brief
             * @return
             */
            [[nodiscard]] char get() const;
        };

        /**
         * @brief
         * @return
         */
        [[nodiscard]] Stepper getStepper() const;

        /**
         * @brief
         * @param start
         * @param end
         * @return
         */
        [[nodiscard]] std::string getString(size_t start, size_t end) const;
    };

    /**
     * @brief
     * @param csize
     */
    explicit ReferenceManager(size_t csize);

    /**
     * @brief
     * @param index
     * @param ref
     */
    void addRef(size_t index, std::unique_ptr<Reference> ref);

    /**
     * @brief
     * @param name
     * @param pos
     * @return
     */
    std::shared_ptr<const std::string> loadAt(const std::string& name, size_t pos);

    /**
     * @brief
     * @param name
     * @param start
     * @param end
     * @return
     */
    ReferenceExcerpt load(const std::string& name, size_t start, size_t end);

    /**
     * @brief
     * @param name
     * @return
     */
    [[nodiscard]] std::vector<std::pair<size_t, size_t>> getCoverage(const std::string& name) const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] std::vector<std::string> getSequences() const;

    /**
     * @brief
     * @param name
     * @return
     */
    size_t getLength(const std::string& name);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_REFERENCE_MANAGER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
