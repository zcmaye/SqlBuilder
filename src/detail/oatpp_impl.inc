#ifdef SUPPORT_OATPP
		template<typename T, std::enable_if_t <is_object_v<T>, int > = 0>
		Assign operator=(T& value) const {
			if constexpr (std::is_same_v<T, oatpp::String>) {
				if (value != nullptr && !value->empty()) {
					return Assign(_name, format_value(*value));
				}
			}
			else {
				if (value != nullptr) {
					return Assign(_name, format_value(*value));
				}
			}
			return Assign();
		}

		template<typename T, std::enable_if_t<is_object_v<T>, int> = 0>
		Condition operator==(T value) const {
			if (value != nullptr) {
				return *this == *value;
			}
			return Condition();
		}
		template<typename T, std::enable_if_t<is_object_v<T>, int> = 0>
		Condition operator!=(T value) const {
			if (value != nullptr) {
				return *this != *value;
			}
			return Condition();
		}
		template<typename T, std::enable_if_t<is_object_v<T>, int> = 0>
		Condition operator>(T value) const {
			if (value != nullptr) {
				return *this > *value;
			}
			return Condition();
		}
		template<typename T, std::enable_if_t<is_object_v<T>, int> = 0>
		Condition operator>=(T value) const {
			if (value != nullptr) {
				return *this >= *value;
			}
			return Condition();
		}
		template<typename T, std::enable_if_t<is_object_v<T>, int> = 0>
		Condition operator<(T value) const {
			if (value != nullptr) {
				return *this < *value;
			}
			return Condition();
		}
		template<typename T, std::enable_if_t<is_object_v<T>, int> = 0>
		Condition operator<=(T value) const {
			if (value != nullptr) {
				return *this <= *value;
			}
			return Condition();
		}

		//ename LIKE '张%'
		Condition like(const oatpp::String& value, std::string_view fmt = "%{}%") const 
		{
			if (value != nullptr && !value->empty()) {
				return like(*value, fmt);
			}
			return Condition();
		}

		template<typename T, std::enable_if_t<is_object_v<T>, int> = 0>
		Condition between_and(const T& lower,const T& upper) const {
			if (lower != nullptr && upper != nullptr) {
				bool is_ok = true;
				if constexpr (std::is_same_v<T, oatpp::String>) {
					is_ok = !lower->empty() && !upper->empty();
				}
				if (is_ok) {
					return Condition(_name, "BETWEEN", std::format("{} AND {}",format_value(*lower),format_value(*upper)));
				}
			}
			return Condition();
		}
#endif
