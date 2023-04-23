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

namespace ps
{

namespace lib
{

namespace sql
{

namespace formatter
{

namespace oracle
{

/**
 * @class cGrammar
 * - A parser specialized to the SQL grammar used in Oracle Database.
 * - When an object is given to a vParse function together
 *   with a SQL statement to be converted, conversion directive
 *   to pretty is stored in the object based on the parsing of SQL.
 */
template <typename Iterator, typename Skipper=bqi::space_type>
class cGrammar
    : public bqi::grammar<Iterator, Skipper>
{
public:
    cGrammar(const Iterator& first);
    /**
     * @return
     *   conversion directive to pretty
     */
    tDirective oGetDirective() const
    {
        return oDirective_;
    }
private:
    void vPrintDebugNode(void);
    tDirective oDirective_;
    phx::function<cErrorHandler> handler_;
    phx::function<cAnnotator<Iterator>> oAnnotator_;

    bqi::symbols<>
        reserved_words, pseudocolumns
        , argumentless_func_name
        , scalar_func_name, grouping_func_name
        , comparator
        ;

    bqi::rule<Iterator, Skipper>
        subquery, query_block, select_list, uniqueness_modifier
        , group_by_clause, having_clause, order_by_clause
        , as_clause, table_reference, query_table_expression, dblink_name
        , join_clause, inner_cross_join_clause, outer_join_clause
        , query_partition_clause
        , confition_list, and_list
        , hierarchical_query_clause
        , bool_expr, binary_expr, between_expr
        , null_expr, like_expr, in_expr, exists_expr
        , expr, simple_expression, unary_left_join
        , compound_expression, assoc_term, assoc_cat, assoc_fctr
        , case_expression, simple_case_expr, searched_case_expr, else_clause
        , function_expression, mandatory_arg_list, optional_arg_list
        , kwd_or, schema_prefix, db_obj_name
        , nonquoted_identifier, quoted_identifier
        , unary_prior, string_literal, const_decimal
        ;

    bqi::rule<Iterator, cKeyword(), Skipper>
        set_operator, kwd_from, where_clause
        , outer_join_type
        , kwd_connect_by, kwd_start_with
        , kwd_and, kwd_group, kwd_order
        , kwd_when, kwd_else, kwd_end
        ;

    bqi::rule<Iterator, cNormDelim(), Skipper> list_separator;

    bqi::rule<Iterator, cIncrDelim(), Skipper> open_prts;

    bqi::rule<Iterator, cDecrDelim(), Skipper> close_prts;

};

template <typename Iterator, typename Skipper>
void cGrammar<Iterator, Skipper>::vPrintDebugNode(void)
{
    BOOST_SPIRIT_DEBUG_NODES(
        (subquery)
        (query_block)
        (select_list)
        (uniqueness_modifier)
        (group_by_clause)
        (kwd_group)
        (having_clause)
        (order_by_clause)
        (kwd_order)
        (set_operator)
        (as_clause)
        (kwd_from)
        (table_reference)
        (query_table_expression)
        (dblink_name)
        (join_clause)
        (inner_cross_join_clause)
        (outer_join_clause)
        (outer_join_type)
        (query_partition_clause)
        (where_clause)
        (hierarchical_query_clause)
        (kwd_connect_by)
        (kwd_start_with)
        (confition_list)
        (and_list)
        (bool_expr)
        (binary_expr)
        (between_expr)
        (null_expr)
        (like_expr)
        (in_expr)
        (exists_expr)
        (kwd_and)
        (kwd_or)
        (unary_prior)
        (expr)
        (simple_expression)
        (unary_left_join)
        (compound_expression)
        (assoc_term)
        (assoc_cat)
        (assoc_fctr)
        (case_expression)
        (kwd_when)
        (kwd_else)
        (kwd_end)
        (simple_case_expr)
        (searched_case_expr)
        (else_clause)
        (function_expression)
        (mandatory_arg_list)
        (optional_arg_list)
        (schema_prefix)
        (db_obj_name)
        (nonquoted_identifier)
        (quoted_identifier)
        (string_literal)
        (const_decimal)
        (list_separator)
        (open_prts)
        (close_prts)
    );
}

template <typename Iterator, typename Skipper>
cGrammar<Iterator, Skipper>::cGrammar(const Iterator& first)
    : cGrammar<Iterator>::base_type(subquery, "subquery")
    , oAnnotator_(first)
{
    using bqi::char_;
    using bqi::alpha;
    using bqi::alnum;
    using bqi::digit;
    using bqi::lit;
    using bqi::no_case;
    using bqi::lexeme;
    using bqi::raw;
    using bqi::no_skip;
    using bqi::_val;
    using bqi::_1;
    using bqi::_2;
    using bqi::_3;
    using bqi::_4;

    reserved_words
        = "access", "add", "all", "alter", "and", "any", "as"
        , "asc", "audit", "between", "by", "case", "char", "check"
        , "cluster", "column", "column_value", "comment"
        , "compress", "connect", "create", "current", "date"
        , "decimal", "default", "delete", "desc", "distinct"
        , "drop", "else", "end", "exclusive", "exists", "file", "float"
        , "for", "from", "grant", "group", "having", "identified"
        , "immediate", "in", "increment", "index", "initial"
        , "insert", "integer", "intersect", "into", "is"
        , "like", "lock", "long", "maxextents", "minus", "mlslabel"
        , "mode", "modify", "nested_table_id", "noaudit", "nocompress"
        , "not", "nowait", "null", "number", "of", "offline", "on"
        , "online", "option", "or", "order", "pctfree", "prior"
        , "privileges", "public", "raw", "rename", "resource"
        , "revoke", "row", "rows", "select"
        , "session", "set", "share", "size", "smallint", "start"
        , "successful", "synonym", "table", "then"
        , "to", "trigger", "uid", "union", "unique", "update"
        , "validate", "values", "varchar", "varchar2"
        , "view", "when", "whenever", "where", "with"
        ;

    pseudocolumns
          // Hierarchical Query Pseudocolumns
        = "connect_by_iscycle", "connect_by_isleaf", "level"
          // Sequence Pseudocolumns
        , "currval", "nextval"
          // Version Query Pseudocolumns
        , "versions_startscn", "versions_starttime", "versions_endscn"
        , "versions_endtime", "versions_xid", "versions_operation"
          // etc.
        , "column_value", "object_id", "object_value"
        , "ora_rowscn" , "rowid", "rownum", "xmldata"
        ;

    argumentless_func_name
        = "current_date", "current_time", "current_timestamp"
        , "dbtimezone", "sessiontimezone", "sysdate"
        , "systimestamp", "user"
        ;

    scalar_func_name
        = "abs", "acos", "add_months", "ascii", "asciistr"
        , "asin", "atan", "atan2", "bfilename", "bin_to_num"
        , "bitand", "cast", "ceil", "chartorowid", "chr"
        , "coalesce", "compose", "concat", "convert", "cos", "cosh"
        , "decode", "decompose", "depth", "dump", "empty_blob"
        , "empty_clob", "existsnode", "exp", "extract", "extractvalue"
        , "floor", "from_tz", "greatest", "hextoraw", "initcap"
        , "instr", "last_day", "least", "length", "ln", "localtimestamp"
        , "log", "lower", "lpad", "ltrim", "mod", "months_between"
        , "new_time", "next_day", "nls_charset_decl_len", "nls_charset_id"
        , "nls_charset_name", "nls_initcap", "nls_lower", "nls_upper"
        , "nlssort", "nullif", "numtodsinterval", "numtoyminterval"
        , "nvl", "nvl2", "path", "power", "rawtohex", "rawtonhex"
        , "replace", "round", "rowidtochar", "rowidtonchar", "rpad"
        , "rtrim", "sign", "sin", "sinh", "soundex", "sqrt", "substr"
        , "sys_connect_by_path", "sys_context", "sys_dburigen"
        , "sys_extract_utc", "sys_guid", "sys_typeid", "sys_xmlagg"
        , "sys_xmlgen", "tan", "tanh", "to_char", "to_clob", "to_date"
        , "to_dsinterval", "to_lob", "to_multi_byte", "to_nchar"
        , "to_nclob", "to_number", "to_single_byte", "to_timestamp"
        , "to_timestamp_tz", "to_yminterval", "translate", "treat"
        , "trim", "trunc", "trunc", "tz_offset", "uid", "unistr"
        , "updatexml", "upper", "userenv", "vsize", "width_bucket"
        , "xmlagg", "xmlcolattval", "xmlconcat", "xmlforest"
        , "xmlsequence", "xmltransform"
        ;

    grouping_func_name
        = "avg", "corr", "count", "covar_pop", "covar_samp", "cume_dist"
        , "dense_rank", "first", "group_id", "grouping", "grouping_id"
        , "last", "max", "min", "percentile_cont", "percentile_disc"
        , "percent_rank", "rank", "regr_", "stddev", "stddev_pop"
        , "stddev_samp", "sum", "var_pop", "var_samp", "variance"
        ;

    comparator
        = "=", "!=", "^=", "<>", ">", "<", ">=", "<=";

    subquery
        = (
            open_prts >> subquery >> close_prts
            | query_block % set_operator
          ) >> -order_by_clause
        ;

    query_block
        = no_case["select"]
            >> -uniqueness_modifier
            >> select_list
            >> kwd_from >> table_reference >>
                -(join_clause | +(list_separator >> table_reference))
            >> -(where_clause >> confition_list)
            >> -(hierarchical_query_clause)
            >> -group_by_clause
        ;

    uniqueness_modifier
        = no_case
        [
            lit("distinct")
            | lit("unique")
            | lit("all")
        ];

    set_operator
        = no_case
        [
            lit("union") >> -lit("all")
            | lit("intersect")
            | lit("minus")
        ];

    group_by_clause
        = kwd_group >> expr % ',' >> -having_clause
        ;

    kwd_group
        = no_case[lit("group") >> "by"]
        ;

    having_clause
        = no_case["having"] >> confition_list
        ;

    order_by_clause
        = kwd_order >> (expr >> -(no_case["asc"] | no_case["desc"])) % list_separator
        ;

    kwd_order
        = no_case[lit("order") >> "by"]
        ;

    select_list
        = (
            expr >> -as_clause
            | -schema_prefix >> db_obj_name >> '*'
        ) % list_separator
        | char_('*')
        ;

    as_clause
       = -no_case["as"] >> db_obj_name
       ;

    kwd_from
        // If you combine "f" and "rom" to derive
        // lit("from"), a compile error will encounter.
        // It seems that it is not good for a rule
        // to be formed with only one parser.
        = no_case[char_('f') >> "rom"]
        ;

    table_reference
        = query_table_expression >> -db_obj_name
        ;

    query_table_expression
        = -schema_prefix >> db_obj_name >> -('@' >> dblink_name)
        | open_prts >> subquery >> close_prts
        ;

    dblink_name
        = lexeme[+(alpha >> *(alnum | char_("_"))) % '.']
        | quoted_identifier
        ;

    join_clause
        = +(inner_cross_join_clause | outer_join_clause)
        ;

    inner_cross_join_clause
        = (
            -no_case["inner"] >> no_case["join"] >> table_reference >> (
                no_case["on"] >> confition_list
                | no_case["using"] >> (-schema_prefix >> db_obj_name) % ','
            )
            | (
                no_case["cross"]
                | no_case["natural"] >> -no_case["inner"]
            ) >> no_case["join"] >> table_reference
        )
        ;

    outer_join_clause
        = -query_partition_clause >> (
            outer_join_type >> no_case["join"]
            | no_case["natural"] >> -outer_join_type >> no_case["join"]
        ) >> table_reference >> -query_partition_clause >> -(
            no_case["on"] >> confition_list
            | no_case["using"] >> (-schema_prefix >> db_obj_name) % ','
        )
        ;

    outer_join_type
        = (no_case["full"] | no_case["left"] | no_case["right"]) >> -no_case["outer"]
        ;

    query_partition_clause
        = no_case["partitiond"] >> no_case["by"] >> (
            open_prts >> expr % list_separator >> close_prts
            | expr % list_separator
        )
        ;

    where_clause
       = no_case[char_('w') >> "here"]
       ;

    hierarchical_query_clause
        // '^' is permutation parser.
        = (kwd_connect_by >> -no_case["nocycle"] >> confition_list)
        ^ (kwd_start_with >> confition_list)
        ;

    kwd_connect_by
       = no_case[lit("connect") >> "by"]
       ;

    kwd_start_with
       = no_case[lit("start") >> "with"]
       ;

    confition_list
        = and_list % kwd_or
        ;

    and_list
        = bool_expr % kwd_and
        ;

    bool_expr
        = -no_case["not"] >> (
            open_prts >> confition_list >> close_prts
            | binary_expr
            | in_expr
            | exists_expr
            | null_expr
            | like_expr
            | between_expr
        );

    binary_expr
        = expr >> comparator >> -unary_prior >> expr
        | -unary_prior >> expr >> comparator >> expr
        ;

    unary_prior
        = no_case["prior"]
        ;

    between_expr
        = expr >> no_case["between"] >> expr >> no_case["and"] >> expr
        ;

    in_expr
        = expr >> -no_case["not"] >> no_case["in"] >> mandatory_arg_list
        ;

    exists_expr
        = no_case["exists"] >> open_prts >> subquery >> close_prts
        ;

    null_expr
        = expr >> no_case["is"] >> -no_case["not"] >> no_case["null"]
        ;

    like_expr
        = expr >> -no_case["not"] >> no_case["like"] >> expr
            >> -(no_case["escape"] >> '\"' >> char_ >> '\"')
        ;

    kwd_and
        = no_case[char_('a') >> "nd"]
        ;

    kwd_or
        = no_case["or"]
        ;

    expr
        = function_expression
        | simple_expression
        | compound_expression
        | case_expression
        ;

    simple_expression
        = -schema_prefix >> (db_obj_name | no_case[pseudocolumns]) >> -unary_left_join
        | -char_("Nn") >> string_literal
        | const_decimal
        | no_case["null"]
        ;

    unary_left_join
        = lit("(+)")
        ;

    compound_expression
        = assoc_term % char_("+-")
        ;

    assoc_term
        = assoc_cat % char_("/*")
        ;

    assoc_cat
        = assoc_fctr % lit("||")
        ;

    assoc_fctr
        = open_prts >> (expr | subquery) >> close_prts
        ;

    case_expression
        = no_case["case"] >> (simple_case_expr | searched_case_expr)
        >> -else_clause >> kwd_end
        ;

    simple_case_expr
        = -schema_prefix >> db_obj_name >> +(kwd_when >> expr >> no_case["then"] >> expr)
        ;

    searched_case_expr
        = +(kwd_when >> confition_list >> no_case["then"] >> expr)
        ;

    else_clause
        = kwd_else >> expr
        ;

    kwd_when
        = no_case[char_('w') >> "hen"]
        ;

    kwd_else
        = no_case[char_('e') >> "lse"]
        ;

    kwd_end
        = no_case[char_('e') >> "nd"]
        ;

    function_expression
        = no_case[scalar_func_name | grouping_func_name] >> optional_arg_list
        | no_case[argumentless_func_name]
        ;

    mandatory_arg_list
        = open_prts >> (expr | subquery) % list_separator >> close_prts
        ;

    optional_arg_list
        = open_prts >> -((expr | subquery) % list_separator) >> close_prts
        ;

    schema_prefix
        = db_obj_name >> '.'
        ;

    db_obj_name
        = nonquoted_identifier
        | quoted_identifier
        ;

    nonquoted_identifier
        = !lexeme[
            no_case[reserved_words | pseudocolumns | argumentless_func_name]
            >> !(alpha | char_("_$#"))
        ]
        >> raw[lexeme[alpha >> *(alnum | char_("_$#"))]]
        ;

    quoted_identifier
        = lexeme[char_('\"') >> +(char_ - char_('\"')) >> char_('\"')]
        ;

    string_literal
        = no_skip
        [
            '\'' >> *(char_ - '\'') >> '\''
            | char_("Qq") >> (
                  lit("\'(") >> *(char_ - char_(")\'")) >> lit(")\'")
                | lit("\'{") >> *(char_ - char_("}\'")) >> lit("}\'")
                | lit("\'[") >> *(char_ - char_("]\'")) >> lit("]\'")
                | lit("\'<") >> *(char_ - char_(">\'")) >> lit(">\'")
            )
        ];

    const_decimal
        = -char_("+-") >> (
                +digit >> -(char_('.') >> *digit)
                | char_('.') >> +digit
            ) >> -('e' >> -char_("+-") >> +digit) >> -char_("fd")
        ;

    list_separator
        = char_(',')
        ;

    open_prts
        = char_('(')
        ;

    close_prts
        = char_(')')
        ;

    reserved_words.name("reserved_words");
    pseudocolumns.name("pseudocolumns");
    argumentless_func_name.name("argumentless_func_name");
    scalar_func_name.name("scalar_func_name");
    grouping_func_name.name("grouping_func_name");
    comparator.name("comparator");

    subquery.name("subquery");
    query_block.name("query_block");
    select_list.name("select_list");
    uniqueness_modifier.name("uniqueness_modifier");
    group_by_clause.name("group_by_clause");
    kwd_group.name("kwd_group");
    having_clause.name("having_clause");
    order_by_clause.name("order_by_clause");
    kwd_order.name("kwd_order");
    set_operator.name("set_operator");
    as_clause.name("as_clause");
    kwd_from.name("kwd_from");
    table_reference.name("table_reference");
    query_table_expression.name("query_table_expression");
    dblink_name.name("dblink_name");
    join_clause.name("join_clause");
    inner_cross_join_clause.name("inner_cross_join_clause");
    outer_join_clause.name("outer_join_clause");
    outer_join_type.name("outer_join_type");
    query_partition_clause.name("query_partition_clause");
    where_clause.name("where_clause");
    hierarchical_query_clause.name("hierarchical_query_clause");
    kwd_connect_by.name("kwd_connect_by");
    kwd_start_with.name("kwd_start_with");
    confition_list.name("confition_list");
    and_list.name("and_list");
    bool_expr.name("bool_expr");
    binary_expr.name("binary_expr");
    between_expr.name("between_expr");
    null_expr.name("null_expr");
    like_expr.name("like_expr");
    in_expr.name("in_expr");
    exists_expr.name("exists_expr");
    kwd_and.name("kwd_and");
    kwd_or.name("kwd_or");
    kwd_when.name("kwd_when");
    kwd_else.name("kwd_else");
    kwd_end.name("kwd_end");
    unary_prior.name("unary_prior");
    expr.name("expr");
    simple_expression.name("simple_expression");
    unary_left_join.name("unary_left_join");
    case_expression.name("case_expression");
    simple_case_expr.name("simple_case_expr");
    searched_case_expr.name("searched_case_expr");
    else_clause.name("else_clause");
    function_expression.name("function_expression");
    mandatory_arg_list.name("mandatory_arg_list");
    optional_arg_list.name("optional_argument_list");
    compound_expression.name("compound_expression");
    assoc_term.name("assoc_term");
    assoc_cat.name("assoc_cat");
    assoc_fctr.name("assoc_fctr");
    schema_prefix.name("schema_prefix");
    db_obj_name.name("db_obj_name");
    nonquoted_identifier.name("nonquoted_identifier");
    quoted_identifier.name("quoted_identifier");
    string_literal.name("string_literal");
    const_decimal.name("const_decimal");
    list_separator.name("list_separator");
    open_prts.name("open_prts");
    close_prts.name("close_prts");

    bqi::on_error<bqi::fail>(subquery, handler_(_1, _2, _3, _4) );

    /*
     * _1 points to the position in the input sequence
     *    before the rule is matched.
     * _3 points to the position in the input sequence
     *    following the last character that was consumed by the rule.
     *
     * See also:
     * https://www.boost.org/doc/libs/1_61_0/libs/spirit/doc/html/spirit/qi/tutorials/mini_xml___error_handling.html
     */
    auto alias = oAnnotator_(_val, _1, _3, phx::ref(oDirective_));
    bqi::on_success(set_operator, alias);
    bqi::on_success(kwd_from, alias);
    bqi::on_success(where_clause, alias);
    bqi::on_success(outer_join_type, alias);
    bqi::on_success(kwd_connect_by, alias);
    bqi::on_success(kwd_start_with, alias);
    bqi::on_success(kwd_and, alias);
    bqi::on_success(kwd_when, alias);
    bqi::on_success(kwd_else, alias);
    bqi::on_success(kwd_end, alias);
    bqi::on_success(kwd_group, alias);
    bqi::on_success(kwd_order, alias);
    bqi::on_success(list_separator, alias);
    bqi::on_success(open_prts, alias);
    bqi::on_success(close_prts, alias);

    vPrintDebugNode();
}

} // ps::lib::sql::formatter::oracle

} // ps::lib::sql::formatter

} // ps::lib::sql

} // ps::lib

} // ps
