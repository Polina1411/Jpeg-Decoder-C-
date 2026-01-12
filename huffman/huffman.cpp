#include <huffman.h>
#include <stdexcept>
#include <vector>
#include <memory>

struct Node {
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
    int value;
    bool is_leaf;
};

class HuffmanTree::Impl {
    std::unique_ptr<Node> root_;
    Node* current_;

    void Build(Node* node, uint8_t depth, const std::vector<std::pair<uint8_t, uint8_t>>& data,
               size_t& idx) {
        if (idx >= data.size()) {
            return;
        }
        if (depth == data[idx].first) {
            node->is_leaf = true;
            node->value = data[idx].second;
            ++idx;
            return;
        }
        node->left = std::make_unique<Node>();
        Build(node->left.get(), depth + 1, data, idx);
        if (idx < data.size() && depth < data[idx].first) {
            node->right = std::make_unique<Node>();
            Build(node->right.get(), depth + 1, data, idx);
        }
    }

public:
    Impl() : root_(nullptr), current_(nullptr) {
    }

    void BuildTree(const std::vector<uint8_t>& code_lengths, const std::vector<uint8_t>& values) {
        if (code_lengths.size() > 16) {
            throw std::invalid_argument("неверный размер");
        }
        size_t total = 0;
        for (auto len : code_lengths) {
            total += len;
        }
        if (total != values.size()) {
            throw std::invalid_argument("не те значения ");
        }
        int slots = 2;
        for (auto len : code_lengths) {
            if (len > slots) {
                throw std::invalid_argument("не вмещается");
            }
            slots = (slots - len) * 2;
        }
        std::vector<std::pair<uint8_t, uint8_t>> leaves;
        size_t idx = 0;
        for (size_t d = 0; d < code_lengths.size(); ++d) {
            for (size_t i = 0; i < code_lengths[d]; ++i) {
                leaves.push_back({static_cast<uint8_t>(d + 1), values[idx++]});
            }
        }
        root_ = std::make_unique<Node>();
        size_t leaf_idx = 0;
        Build(root_.get(), 0, leaves, leaf_idx);
        if (leaf_idx != leaves.size()) {
            throw std::invalid_argument("неправильная структура");
        }
        current_ = root_.get();
    }

    bool Move(bool bit, int& value) {
        if (!current_) {
            throw std::invalid_argument("дерево не построено");
        }
        current_ = bit ? current_->right.get() : current_->left.get();
        if (!current_) {
            throw std::invalid_argument("ветки не существует");
        }
        if (current_->is_leaf) {
            value = current_->value;
            current_ = root_.get();
            return true;
        }
        return false;
    }
};

HuffmanTree::HuffmanTree() : impl_(std::make_unique<Impl>()) {
}

void HuffmanTree::Build(const std::vector<uint8_t>& code_lengths,
                        const std::vector<uint8_t>& values) {
    impl_->BuildTree(code_lengths, values);
}

bool HuffmanTree::Move(bool bit, int& value) {
    return impl_->Move(bit, value);
}

HuffmanTree::HuffmanTree(HuffmanTree&&) = default;
HuffmanTree& HuffmanTree::operator=(HuffmanTree&&) = default;
HuffmanTree::~HuffmanTree() = default;