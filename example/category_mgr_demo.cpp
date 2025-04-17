#include <spark/spark.hpp>

#include <spark/core/category_manager.hpp>
#include <spark/core/detector.hpp>
#include <spark/core/task_manager.hpp>
#include <spark/core/writer_tree.hpp>
#include <spark/parameters/container.hpp>
#include <spark/parameters/parameters_memory_source.hpp>

#include "example_categories.hpp"

class empty_calibration : public spark::task
{
public:
    using task::task;

    auto init() -> bool override
    {
        cat_raw = model()->get_category(ExampleCategories::ExampleRaw);

        if (cat_raw == nullptr) {
            spdlog::critical("[{}] No SabatRaw category", __PRETTY_FUNCTION__);
            return false;
        }

        cat_cal = model()->build_category<ExampleCal>(ExampleCategories::ExampleCal);

        if (cat_cal == nullptr) {
            spdlog::critical("[{}] Cannot build SabatCal category", __PRETTY_FUNCTION__);
            return false;
        }

        pm_cal = db()->get_container<ExampleCalPar>("ExampleCalPar");
        pm_cal->print();

        return true;
    }

    auto execute() -> bool override
    {
        auto n_objs = cat_raw->get_entries();

        for (int i = 0; i < n_objs; ++i) {
            auto loc = cat_raw->get_locator(i);

            auto raw_obj = cat_raw->get_object<ExampleRaw>(i);

            auto new_cal_obj = cat_cal->make_object_unsafe<ExampleCal>(loc);

            fmt::print("CAL Addr: {}\n", std::tuple {raw_obj->board, raw_obj->channel});

            auto [slope, offset] = pm_cal->get({raw_obj->board, raw_obj->channel});

            fmt::print("CAL Row: {} {}\n", slope, offset);

            new_cal_obj->board = raw_obj->board;
            new_cal_obj->channel = raw_obj->channel;

            new_cal_obj->toa = raw_obj->toa * 0.515;
            new_cal_obj->energy = raw_obj->tot * 0.123 + 456.;
        }

        // std::ranges::for_each(*cat_cal,
        //                       [&](auto& raw_obj)
        //                       {
        //                           // FIXME
        //                       });

        return true;
    }

private:
    spark::category* cat_raw {nullptr};
    spark::category* cat_cal {nullptr};

    spark::container_wrapper<ExampleCalPar> pm_cal;
};

class example_detector : public spark::detector
{
public:
    using detector::detector;

    auto setup_categories(spark::category_manager& cat_mgr) -> void override
    {
        cat_mgr.register_category(ExampleCategories::ExampleRaw, "ExampleRaw", {64}, true);
        cat_mgr.register_category(ExampleCategories::ExampleCal, "ExampleCal", {64}, true);
        // cat_mgr.register_category(ExampleCategories::Dummy3d, "Dummy3", {2, 32}, true);
    }

    auto setup_containers(spark::database& rundb) -> void override
    {
        rundb.register_container<ExampleLookup>("ExampleLookup", 0x0000, 0x1000, 64, "{:x} {}", "{} {}");
        rundb.register_container<ExampleCalPar>("ExampleCalPar", "{:x} {}", "{} {}");
    }

    auto setup_tasks(spark::task_manager& task_mgr) -> void override
    {
        task_mgr.add_task<empty_calibration>();
        // task_mgr.add_task<sabat_clustering, sabat_calibration>();
    }
};

auto main() -> int
{
    auto sprk = spark::sparksys::create<ExampleCategories>();

    sprk.system().make_detector<example_detector>("ExampleDetector");

    auto mem_source = std::make_unique<spark::parameters_memory_source>();
    mem_source->add_view("ExampleLookup",
                         spark::container::ascii_snapshot {"0   0    1   9",
                                                           "0   1    2   19",
                                                           "0   2    1   10",
                                                           "0   3    2   20",
                                                           "0   4    2   11",
                                                           "0   5    1   9",
                                                           "0   6    2   5"});
    mem_source->add_view("ExampleCalPar",
                         spark::container::ascii_snapshot {"0   0    1   9",
                                                           "0   1    2   19",
                                                           "0   2    1   10",
                                                           "0   3    2   20",
                                                           "0   4    2   11",
                                                           "0   5    1   9",
                                                           "0   6    2   5"});

    sprk.pardb().add_source(mem_source.get());

    auto writer = sprk.create_writer<spark::writer::tree>("T", "example_output.root", 0);
}
