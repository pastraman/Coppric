" Vim syntax file for Coppric (.cpr)
if exists("b:current_syntax")
  finish
endif

syntax keyword cprChipKeyword   chip end
syntax keyword cprStructure     in out parts
syntax keyword cprWire          wire
syntax keyword cprConstants     true false

syntax match cprNumber /\<\d\+\>/
syntax match cprBracket /[\[\]()]/
syntax match cprColon /:/
syntax match cprComment /#.*$/
syntax match cprGateName /\<[A-Z][A-Za-z0-9_]*\>/
syntax match cprParamName /[a-zA-Z_][a-zA-Z0-9_]*\ze\s*=/
syntax match cprEquals /=/

highlight cprChipKeyword   ctermfg=Magenta     guifg=#c678dd
highlight cprStructure     ctermfg=Yellow      guifg=#e5c07b
highlight cprWire          ctermfg=Green       guifg=#98c379
highlight cprConstants     ctermfg=Red         guifg=#e06c75
highlight cprNumber        ctermfg=Cyan        guifg=#56b6c2
highlight cprBracket       ctermfg=Gray        guifg=#808080
highlight cprColon         ctermfg=White       guifg=#abb2bf
highlight cprComment       ctermfg=DarkGray    guifg=#5c6370   cterm=italic gui=italic
highlight cprGateName      ctermfg=Blue        guifg=#61afef
highlight cprParamName ctermfg=LightBlue guifg=#56b6c2
highlight cprEquals        ctermfg=White       guifg=#abb2bf

let b:current_syntax = "cpr"
