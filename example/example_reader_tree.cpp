#include "spark/core/reader_tree.hpp"

#include "example_categories.hpp"

auto main() -> int
{
    auto sprk = spark::sparksys::create<ExampleCategories>();

    auto reader = sprk.create_reader<spark::reader::tree>("T");
    reader.add_file("output_sabat.root");

    auto* chain = reader.chain();
    chain->Print();

    reader.set_input({ExampleCategories::ExampleRaw});

    auto cat_fibers_raw = reader.model().get_category(ExampleCategories::ExampleRaw);

    for (auto i = 0l; i < chain->GetEntries(); ++i) {
        reader.get_entry(i);
        cat_fibers_raw->print();
    }
}
