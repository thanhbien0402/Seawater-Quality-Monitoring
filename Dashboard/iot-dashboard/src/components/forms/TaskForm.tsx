'use client'

import { zodResolver } from '@hookform/resolvers/zod'
import { useForm } from 'react-hook-form'
import * as z from 'zod'

import { useToast } from '@/hooks/use-toast'
import { cn, generateUniqueNumber } from "@/lib/utils"

import {
    Form,
    FormControl,
    FormDescription,
    FormField,
    FormItem,
    FormLabel,
    FormMessage
} from '@/components/ui/form'
import { Button } from "@/components/ui/button"
import { Input } from "@/components/ui/input"
import {
    Select,
    SelectContent,
    SelectItem,
    SelectTrigger,
    SelectValue,
} from "@/components/ui/select"
import { Switch } from "@/components/ui/switch"
import { addTask, sendDataTb } from '@/actions'
import { Calendar } from "@/components/ui/calendar"
import { Popover, PopoverContent, PopoverTrigger } from '../ui/popover'
import { CalendarIcon } from 'lucide-react'
import { format, formatDate } from 'date-fns'
import { useTransition } from 'react'
import { Icons } from '@/components/icons'

const formSchema = z.object({
    time: z.string({
        message: "Please select a time to config.",
    }),
    type: z.string({
        message: "Please select a type to config.",
    }),
    trigger_type: z.boolean(),
    // dot: z.date({
    //     required_error: "A date of birth is required.",
    // })
    dot: z.date().optional().refine(d => d === undefined || d instanceof Date, "Invalid date")
})

export const TaskForm = ({ setIsOpen, deviceId }: { setIsOpen: any, deviceId: string }) => {
    const { toast } = useToast()
    const [isPending, startTransition] = useTransition()
    // const [date, setDate] = useState<Date | undefined>()

    const form = useForm<z.infer<typeof formSchema>>({
        resolver: zodResolver(formSchema),
        defaultValues: {
            trigger_type: false,
            dot: undefined
        }
    })

    function onSubmit(values: z.infer<typeof formSchema>) {
        // Do something with the form values.
        // ✅ This will be type-safe and validated.
        startTransition(async () => {
            const { time, type, trigger_type, dot } = values
            const id = Math.abs(generateUniqueNumber())

            console.log(values)
            if (time) {
                const task = {
                    task_date: dot ? formatDate(dot, 'yyyy-MM-dd') : 'Repeat each day',
                    task_time: time,
                    task_type: type,
                    trigger_type: trigger_type,
                    status: "pending",
                    rpcID: id,
                    deviceId
                }


                const token = JSON.parse(JSON.stringify(localStorage.getItem('token')))

                // console.log('formatDate >>>', formatDate(dot, 'yyyy-MM-dd'))

                const { data, error } = await addTask(task)
                const res = await sendDataTb(type, 'add', token, deviceId, id, dot, time, trigger_type)
                // console.log('res >>>', res)

                // console.log('deviceID', deviceId)
                if (error) {
                    console.log('error >>>>>', error)
                }
                else {
                    toast({
                        title: "You submitted the following values:",
                        description: (
                            <pre className="mt-2 w-[340px] rounded-md bg-slate-950 p-4">
                                <p className="text-white">Submitted Successfully</p>
                            </pre>
                        ),
                    })
                }
            }
            form.reset()
            // setDate(undefined)
            setIsOpen(false)
        })
    }

    return (
        <Form {...form}>
            <form onSubmit={form.handleSubmit(onSubmit)} className="space-y-8">
                <FormField
                    control={form.control}
                    name="type"
                    render={({ field }) => (
                        <FormItem>
                            <FormLabel>Components</FormLabel>
                            <Select onValueChange={field.onChange} defaultValue={field.value}>
                                <FormControl>
                                    <SelectTrigger>
                                        <SelectValue placeholder="Select a component to add task" />
                                    </SelectTrigger>
                                </FormControl>
                                <SelectContent>
                                    <SelectItem value="power">Power</SelectItem>
                                    <SelectItem value="relay">Relay</SelectItem>
                                </SelectContent>
                            </Select>
                            <FormDescription>
                                Components in devices
                            </FormDescription>
                            <FormMessage />
                        </FormItem>
                    )}
                />
                <FormField
                    control={form.control}
                    name="trigger_type"
                    render={({ field }) => (
                        <FormItem className="flex flex-row items-center justify-between">
                            <div className="space-y-0.5">
                                <FormLabel className="text-base">
                                    Status
                                </FormLabel>
                                <FormDescription>
                                    Status of Component (On or off)
                                </FormDescription>
                            </div>
                            <FormControl>
                                <Switch
                                    checked={field.value}
                                    onCheckedChange={field.onChange}
                                />
                            </FormControl>
                        </FormItem>
                    )}
                />
                <FormField
                    control={form.control}
                    name="dot"
                    render={({ field }) => (
                        <FormItem className="flex flex-col">
                            <FormLabel>Date of Task</FormLabel>
                            <FormControl>
                                {/* <Input type='date' {...field} /> */}
                                <Popover>
                                    <PopoverTrigger asChild>
                                        <Button
                                            variant={"outline"}
                                            className={cn(
                                                "w-full justify-start text-left font-normal",
                                                !field.value && "text-muted-foreground"
                                            )}
                                        >
                                            <CalendarIcon />
                                            {field.value ? format(field.value, "PPP") : <span>Pick a date</span>}
                                        </Button>
                                    </PopoverTrigger>
                                    <PopoverContent className="w-auto p-0 pointer-events-auto" align="start">
                                        <Calendar
                                            className='pointer-events-auto'
                                            mode="single"
                                            selected={field.value}
                                            onSelect={field.onChange}
                                            disabled={{ before: new Date() }}
                                        />
                                    </PopoverContent>
                                </Popover>
                            </FormControl>
                            <FormDescription>
                                Date of specific task to invoke
                            </FormDescription>
                            <FormMessage />
                        </FormItem>
                    )}
                />
                <FormField
                    control={form.control}
                    name="time"
                    render={({ field }) => (
                        <FormItem>
                            <FormLabel>Time</FormLabel>
                            <FormControl>
                                <Input type='time' {...field} />
                            </FormControl>
                            <FormDescription>
                                Choose the specific time to invoke task
                            </FormDescription>
                            <FormMessage />
                        </FormItem>
                    )}
                />
                <Button type="submit">Submit <Icons.spinner
                    className={cn('animate-spin', { hidden: !isPending })}
                /></Button>
            </form>
        </Form>
    )
}